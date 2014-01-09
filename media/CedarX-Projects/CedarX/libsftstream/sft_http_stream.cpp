/*******************************************************************************
--                                                                            --
--                    CedarX Multimedia Framework                             --
--                                                                            --
--          the Multimedia Framework for Linux/Android System                 --
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                         Softwinner Products.                               --
--                                                                            --
--                   (C) COPYRIGHT 2011 SOFTWINNER PRODUCTS                   --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
*******************************************************************************/
//#define LOG_NDEBUG 0
#define LOG_TAG "sft_http_stream"
#include <CDX_Debug.h>

#include <CDX_Common.h>
#include <cedarx_stream.h>
#include <cedarx_demux.h>

#include <HTTPBase.h>
#include <NuCachedSource2.h>
#include <media/stagefright/MediaDefs.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/foundation/hexdump.h>
#include <DRMExtractor.h>
#include <WVMExtractor.h>
#include <media/stagefright/FileSource.h>
#include <drm/drm_framework_common.h>

//#define SAVE_ENCRYPTED_DATA

namespace android {

class SftHttpStream {
public:
	SftHttpStream();
	virtual ~SftHttpStream();

	virtual int Open(CedarXDataSourceDesc *datasrc_desc);
	virtual int Close();
	virtual int Sniff(bool sniffVideo = false, CedarXDataSourceDesc *datasrc_desc = NULL);
	virtual int Read(void *ptr, size_t size);
	virtual int GetCacheState(CDX_CACHE_STATE *cdx_cache_state);
	virtual int Seek(long long offset, int whence);
	virtual void SetDuration(int64_t duration);
	virtual int Decrypt(void *ptr, size_t size, int pkt_type);
	virtual int64_t Tell();
	virtual int64_t GetSize();
	virtual void Reset();
	virtual void Disconnect();
	virtual void SetTrackInfo(int32_t trackNum);
    virtual status_t generalInterface(int32_t cmd, int32_t ext1, int32_t ext2);
private:
	/*
	 * value in NuCachedSource2.h
	 * enum {
	 *     kPageSize                       = 65536,
	 *     kDefaultHighWaterThreshold      = 20 * 1024 * 1024,//MBs
	 *     kDefaultLowWaterThreshold       = 4 * 1024 * 1024,//MBs
	 *
	 *     // Read data after a 15 sec timeout whether we're actively
	 *     // fetching or not.
	 *     kDefaultKeepAliveIntervalUs     = 15000000,//us
	 *  };
	 *
    */
    enum {
    	//set kDefaultLowWaterThreshold Higher than
    	//max cache size in CedarX, otherwise it may cause a
    	//dead loop.
    	kDefaultLowWaterThreshold       = 256 * 1024,//KBs

    	//Keep following two enums accordant with NuCahcedSource2.h,
    	//keep in mind of the different units.
        kDefaultHighWaterThreshold      = 500 * 1024,//KBs

        // Read data after a 15 sec timeout whether we're actively
        // fetching or not.
        kDefaultKeepAliveIntervalUs     = 15,//secs
    };

    enum State {
        DISCONNECTED,
        CONNECTING,
        CONNECTED,
        READING,
        DISCONNECTING,
    };

    class TrackInfo {
    public:
    	TrackInfo(const sp<DecryptHandle> &decryptHandle,
                DrmManagerClient *managerClient,
                int32_t trackId, DrmBuffer *ipmpBox);

        ~TrackInfo();

        int32_t decrypt(void *ptr,  size_t size);

    private:
        sp<DecryptHandle> mDecryptHandle;
        DrmManagerClient* mDrmManagerClient;
        size_t mTrackId;
        mutable Mutex mTrackLock;

#ifdef SAVE_ENCRYPTED_DATA
		int mFdEncrypted;
		int mFdDecrypted;
#endif
        TrackInfo(const TrackInfo &);
        TrackInfo &operator=(const TrackInfo &);
    };

    TrackInfo *mVideoTrack;
    TrackInfo *mAudioTrack;

	sp<DataSource> dataSource;
	sp<HTTPBase> mConnectingDataSource;
	sp<NuCachedSource2> mCachedSource;
    DrmManagerClient *mDrmManagerClient;
    sp<DecryptHandle> mDecryptHandle;
	bool mUIDValid;
	uid_t mUID;
	int64_t mFilePos;
	int64_t mFileLen;
	int64_t mBitrate;
	int64_t mDurationUs;
	String8 mUri;
    KeyedVector<String8, String8> mUriHeaders;
	mutable Mutex mLock;
	bool    mHasRegisterSniffers;
	bool 	mForceDisconnect;
	int32_t mState;
	int32_t mTrackCount;

	status_t getEstimatedBandwidthKbps(int32_t *kbps);
};

SftHttpStream::TrackInfo::TrackInfo(const sp<DecryptHandle> &decryptHandle,
        DrmManagerClient *managerClient,
        int32_t trackId, DrmBuffer *ipmpBox)
    : mDecryptHandle(decryptHandle),
      mDrmManagerClient(managerClient),
      mTrackId(trackId) {
	CHECK(mDrmManagerClient);

	status_t ret = mDrmManagerClient->initializeDecryptUnit(
			mDecryptHandle, trackId, ipmpBox);
	LOGV("init ret %d, mTrackId %d", ret, mTrackId);
#ifdef SAVE_ENCRYPTED_DATA
	char path [32];
	sprintf(path, "/data/camera/enc_%d.dat", mTrackId);
	mFdEncrypted = open(path, O_CREAT | O_RDWR, 0644);
	sprintf(path, "/data/camera/dec_%d.dat", mTrackId);
	mFdDecrypted = open(path, O_CREAT | O_RDWR, 0644);;
#endif
}

status_t SftHttpStream::TrackInfo::decrypt(void *ptr, size_t size) {
    Mutex::Autolock autoLock(mTrackLock);
    status_t err;
    DrmBuffer encryptedDrmBuffer((char *)ptr, size);
    DrmBuffer decryptedDrmBuffer;
    decryptedDrmBuffer.length = size;
    decryptedDrmBuffer.data = new char[size];
    DrmBuffer *pDecryptedDrmBuffer = &decryptedDrmBuffer;

#ifdef SAVE_ENCRYPTED_DATA
		if(mFdEncrypted >= 0) {
			write(mFdEncrypted, ptr, size);
		}
#endif

    if ((err = mDrmManagerClient->decrypt(mDecryptHandle, mTrackId,
            &encryptedDrmBuffer, &pDecryptedDrmBuffer)) != NO_ERROR) {

        if (decryptedDrmBuffer.data) {
            delete [] decryptedDrmBuffer.data;
            decryptedDrmBuffer.data = NULL;
        }
        LOGV("decrypt failed with %d", err);
        return err;
    }
    CHECK(pDecryptedDrmBuffer == &decryptedDrmBuffer);

#ifdef SAVE_ENCRYPTED_DATA
	if(mFdDecrypted >= 0) {
		write(mFdDecrypted, ptr, size);
	}
#endif
    memcpy(ptr, decryptedDrmBuffer.data, decryptedDrmBuffer.length);

    if (decryptedDrmBuffer.data) {
        delete [] decryptedDrmBuffer.data;
        decryptedDrmBuffer.data = NULL;
    }
    return OK;
}

SftHttpStream::TrackInfo::~TrackInfo() {
	Mutex::Autolock autoLock(mTrackLock);
	mDrmManagerClient->finalizeDecryptUnit(mDecryptHandle, mTrackId);
#ifdef SAVE_ENCRYPTED_DATA
	if(mFdEncrypted >= 0) {
		close(mFdEncrypted);
	}

	if(mFdDecrypted >= 0) {
		close(mFdDecrypted);
	}
#endif
}


SftHttpStream::SftHttpStream():
		mVideoTrack(NULL),
		mAudioTrack(NULL),
		mFilePos(0),
		mFileLen(0),
		mBitrate(25000),
		mDurationUs(0),
		mHasRegisterSniffers(false),
		mForceDisconnect(false),
		mState(DISCONNECTED),
		mTrackCount(0) {

	mUriHeaders.clear();
}

SftHttpStream::~SftHttpStream()
{
	if(mVideoTrack) {
		delete mVideoTrack;
	}

	if(mAudioTrack) {
		delete mAudioTrack;
	}

}

int SftHttpStream::Open(CedarXDataSourceDesc *datasrc_desc)
{
	int flags = 0;

    bool isWidevineStreaming = false;
    AString sniffedMIME;
    mUri = datasrc_desc->source_url;

    if (!strncasecmp("widevine://", mUri.string(), 11)) {
        isWidevineStreaming = true;
        String8 newURI = String8("http://");
        newURI.append(mUri.string() + 11);

        mUri = newURI;
    }

	if (!strncasecmp("http://", mUri.string(), 7)
			|| !strncasecmp("https://", mUri.string(), 8)
			|| isWidevineStreaming) {

		if(strstr(mUri.string(),"m3u8")  ||
			strstr(mUri.string(),"espn") ||
			strstr(mUri.string(), "iask"/*sina video*/)) {
			flags |= HTTPBase::kFlagUAIPAD;
		}

		if(datasrc_desc->url_headers) {
			mUriHeaders = *(KeyedVector<String8, String8> *)datasrc_desc->url_headers;
	        ssize_t index = mUriHeaders.indexOfKey(String8("x-hide-urls-from-log"));
	        if (index >= 0) {
	            // Browser is in "incognito" mode, suppress logging URLs.

	            // This isn't something that should be passed to the server.
	            mUriHeaders.removeItemsAt(index);
	            flags |= HTTPBase::kFlagIncognito;
	        }
		}

		mConnectingDataSource = HTTPBase::Create(flags /*(mFlags & INCOGNITO)
					? HTTPBase::kFlagIncognito
					: 0*/);

		if (mUIDValid) {
			mConnectingDataSource->setUID(mUID);
		}

		String8 cacheConfig;
		bool disconnectAtHighwatermark;
		NuCachedSource2::RemoveCacheSpecificHeaders(
				&mUriHeaders, &cacheConfig, &disconnectAtHighwatermark);

		mLock.unlock();
		mState = CONNECTING;
		if(mForceDisconnect) {
			LOGI("force disconnect, return error");
			return ERROR_IO;
		}
		//keep this statement before connection
		dataSource = mConnectingDataSource;

		status_t err = mConnectingDataSource->connect(mUri, &mUriHeaders);
		if(err == OK && mConnectingDataSource->isRedirected()) {
			mConnectingDataSource->disconnect();
			AString redirectUrl = mConnectingDataSource->getRedirectUri(true);
			LOGI("redirect url %s", redirectUrl.c_str());
			if(mForceDisconnect) {
				LOGI("force disconnect, redirect return error");
				return ERROR_IO;
			}
			if(!redirectUrl.empty()) {
				err = mConnectingDataSource->connect(redirectUrl.c_str(), &mUriHeaders);
			}
		}
		mLock.lock();

		if (err != OK) {
			LOGI("mConnectingDataSource->connect() returned %d", err);
			mConnectingDataSource.clear();
			return err;
		}
		mState = CONNECTED;

        if (!isWidevineStreaming) {
            // The widevine extractor does its own caching.
        	if(cacheConfig.isEmpty()) {
        		char s[20];
        		sprintf(s, "%ld/%ld/%d", (long int)kDefaultLowWaterThreshold,
        				(long int)kDefaultHighWaterThreshold, kDefaultKeepAliveIntervalUs);
        		cacheConfig.append(s);
        	}

        	LOGI("set cache-config as: %s", cacheConfig.string());
            mCachedSource = new NuCachedSource2(
                    mConnectingDataSource,
                    cacheConfig.isEmpty() ? NULL : cacheConfig.string(),
                    disconnectAtHighwatermark);

            dataSource = mCachedSource;

			datasrc_desc->sft_cached_source2 = mCachedSource.get();
			mConnectingDataSource.clear();
        } else {
    		datasrc_desc->sft_http_source = mConnectingDataSource.get();
        }

        if (dataSource == NULL) {
            return UNKNOWN_ERROR;
        }

        Sniff(true/*sniffVideo*/, datasrc_desc);
	} else {

		dataSource = new FileSource(mUri.string());
		Sniff(true/*sniffVideo*/, datasrc_desc);

        dataSource->getDrmInfo(mDecryptHandle, &mDrmManagerClient);

        if (mDecryptHandle != NULL) {
            CHECK(mDrmManagerClient);
            if (RightsStatus::RIGHTS_VALID != mDecryptHandle->status) {
            	return ERROR_DRM_NO_LICENSE;
            }
        } else {
        	return ERROR_UNSUPPORTED;
        }
	}

    String8 contentType = dataSource->getMIMEType();
    //get file length
    dataSource->getSize(&mFileLen);
    LOGV("stream length %lld, content type %s", mFileLen, contentType.string());
    datasrc_desc->sft_stream_length = mFileLen;


	if(dataSource != NULL
			&& mForceDisconnect) {
		dataSource->forceDisconnect();
	}

	return 0;
}

int SftHttpStream::Close()
{
	if(mConnectingDataSource != NULL
			&& mState != DISCONNECTED){
		mState = DISCONNECTING;
		mConnectingDataSource->disconnect();
		mState = DISCONNECTED;
	}

	mCachedSource.clear();
	dataSource.clear();

	return 0;
}

void SftHttpStream::Disconnect()
{
	mForceDisconnect = true;
	if(dataSource != NULL) {
		LOGI("disconnect connection");
		dataSource->forceDisconnect();
	}
}

int SftHttpStream::Sniff(bool sniffVideo, CedarXDataSourceDesc *datasrc_desc)
{
	float confidence;
	String8 mimeType;
	sp<AMessage> dummy;

	if(sniffVideo) {
		bool success = SniffDRM(dataSource, &mimeType, &confidence, &dummy);
		LOGV("SniffDRM:success %d, mime %s", success, mimeType.string());
		if(success && !strncmp(mimeType.string(), "drm+es_based+", 13)) {
			LOGV("es based drm video.");
			datasrc_desc->media_type = CEDARX_MEDIATYPE_OMA_DRM_VIDEO;
		} else if(success && !strncmp(mimeType.string(), "drm+container_based+", 20)) {
			LOGV("container based drm video.");
			datasrc_desc->media_type = CEDARX_MEDIATYPE_OMA_DRM_VIDEO;
		} else
#if (CEDARX_ANDROID_VERSION < 7)
			if (success && !strcasecmp(mimeType.string(), MEDIA_MIMETYPE_CONTAINER_WVM)) {
				//This case is valid only on Android4.0.
				LOGI("libs of wvm exisit");
				datasrc_desc->media_type = CEDARX_MEDIATYPE_WVM_VIDEO;
			}
#else
			if(!success) {
				success = SniffWVM(dataSource, &mimeType, &confidence, &dummy);
				LOGV("SniffWVM:success %d, mime %s", success, mimeType.string());
				if (success &&
					 !strcasecmp(mimeType.string(), MEDIA_MIMETYPE_CONTAINER_WVM)) {
					LOGI("libs of wvm exisit");
					datasrc_desc->media_type = CEDARX_MEDIATYPE_WVM_VIDEO;
				}
			}
#endif
	} else {
		if(!mHasRegisterSniffers) {
			DataSource::RegisterDefaultSniffers();
			mHasRegisterSniffers = true;
		}

		if (!dataSource->sniff(&mimeType, &confidence, &dummy)) {
			return -1;
		}

		LOGI("Sniff MIME:%s",mimeType.string());
	}
	return 0;
}

int SftHttpStream::Read(void * ptr, size_t size)
{
	ssize_t readBytes;

	if(mFileLen > 0 && mFilePos > mFileLen) {
		LOGW("read beyond file");
		return -1;
	}

	if(mForceDisconnect) {
		return -1;
	}

	mState = READING;
	readBytes = dataSource->readAt(mFilePos, ptr, size);
	mState = CONNECTED;
//	LOGV("require size %x, file Pos %llx, readBytes %x", size, mFilePos, readBytes);
	mFilePos += readBytes;
	return readBytes;
}

int SftHttpStream::Seek(long long offset, int whence)
{
	int ret = 0;
	if(whence == SEEK_CUR) {
		mFilePos += offset;
	} else if(whence == SEEK_SET) {
		mFilePos = offset;
	} else if(whence == SEEK_END) {
		mFilePos = mFileLen + offset;
	} else {
		ret = -1;
	}

	if(mFilePos < 0)
		mFilePos = 0;

	//seek beyond end of file
	if((mFileLen > 0) && (mFilePos > mFileLen)) {
		mFilePos = mFileLen + 1;
		ret = -1;
	}

	if(!ret && (mFilePos < mFileLen || !mFileLen)) {
		uint8_t buf;
		//start to cache except it's end of stream.
		int32_t readBytes = dataSource->readAt(mFilePos, &buf, 0);
	}

	return ret;
}

int64_t SftHttpStream::Tell()
{
	return mFilePos;
}

int64_t SftHttpStream::GetSize()
{
	return mFileLen;
}

void SftHttpStream::Reset()
{
	mFilePos = 0;
}

void SftHttpStream::SetDuration(int64_t duration)
{
	LOGV("set duration %lld", duration);
	mDurationUs = duration;
	mBitrate 	= mFileLen * 8000000ll / mDurationUs;
}

status_t SftHttpStream::getEstimatedBandwidthKbps(int32_t *kbps)
{
	if(mCachedSource != NULL) {
		return mCachedSource->getEstimatedBandwidthKbps(kbps);
	} else if(mConnectingDataSource != NULL) {
		if(mConnectingDataSource->estimateBandwidth(kbps)) {
        	*kbps /= 1000;
        	return OK;
		}
	}
	return ERROR_UNSUPPORTED;
}

int SftHttpStream::GetCacheState(CDX_CACHE_STATE *cdx_cache_state)
{
    status_t finalStatus;
    double percentage = 0;
    memset(cdx_cache_state, 0, sizeof(CDX_CACHE_STATE));
    size_t cachedDataRemaining = 0;
	if (mCachedSource != NULL) {
		cachedDataRemaining  = mCachedSource->approxDataRemaining(&finalStatus);
    } else {
    	//Should not be here. As far, only widevine does its own cache.
    	//We handle this case in widevine demux.
    	return ERROR_UNSUPPORTED;
	}

	if(mFileLen) {
		size_t cachedSize = mCachedSource->cachedSize();
		percentage = 100.0 * (double)(cachedSize)/mFileLen;
	} else if(mBitrate) {
		size_t cachedSize = mCachedSource->cachedSize();
		int64_t cachedDurationUs = cachedSize * 8000000ll / mBitrate;
		percentage = 100.0 * (double)cachedDurationUs / mDurationUs;
	}

	if (percentage > 100) {
		percentage = 100;
	}
	cdx_cache_state->buffering_percent  = (int)percentage;
    cdx_cache_state->filled_size 		= cachedDataRemaining;
    cdx_cache_state->eof_flag 			= (finalStatus != OK);
	int32_t kbps = 0;

    if(getEstimatedBandwidthKbps(&kbps) == OK) {
    	cdx_cache_state->bandwidth_kbps = kbps;
    }

	LOGV("filled size:%d, eof_flag %d, bandwidth:%dkbps, percentage %d",
			cdx_cache_state->filled_size, cdx_cache_state->eof_flag,
			cdx_cache_state->bandwidth_kbps, cdx_cache_state->buffering_percent);
	return OK;
}

status_t SftHttpStream::generalInterface(int32_t cmd, int32_t ext1, int32_t ext2) {

	if(mCachedSource != NULL) {
		return mCachedSource->generalInterface(cmd, ext1, ext2);
	}
	return ERROR_UNSUPPORTED;
}

int SftHttpStream::Decrypt(void *ptr, size_t size, int pkt_type) {

	CHECK(ptr != NULL);

	int32_t ret = -1;
//	LOGV("decrypt:[%p, %d, %d]", ptr, size, pkt_type);
	if(CDX_PacketVideo == pkt_type) {
		CHECK(mVideoTrack != NULL);
		ret = mVideoTrack->decrypt(ptr, size);
	} else if(CDX_PacketAudio == pkt_type){
		CHECK(mAudioTrack != NULL);
		ret = mAudioTrack->decrypt(ptr,size);
	}

	return ret;
}

void SftHttpStream::SetTrackInfo(int32_t trackNum) {
	LOGV("SetTrackInfo %08x", trackNum);

	if(trackNum & 0xffff0000) {
		if(mVideoTrack) {
			delete mVideoTrack;
		}
		DrmBuffer ipmpBox;
		mVideoTrack = new TrackInfo(mDecryptHandle,
				mDrmManagerClient, mTrackCount++, &ipmpBox);
	}

	if(trackNum & 0xffff) {
		if(mAudioTrack) {
			delete mAudioTrack;
		}
		DrmBuffer ipmpBox;
		mAudioTrack =  new TrackInfo(mDecryptHandle,
				mDrmManagerClient, mTrackCount++, &ipmpBox);
	}
}

extern "C" {

static int stream_seek(struct cdx_stream_info *stream, cdx_off_t offset, int whence)
{
	SftHttpStream *sft_stream;
	if(stream == NULL)
		return -1;

	sft_stream = (SftHttpStream *)stream->data_src_desc.sft_stream_handle;
	return sft_stream->Seek(offset, whence);
}

static cdx_off_t stream_tell(struct cdx_stream_info *stream)
{
	SftHttpStream *sft_stream;
	if(stream == NULL)
		return -1;

	sft_stream = (SftHttpStream *)stream->data_src_desc.sft_stream_handle;
	return sft_stream->Tell();
}

static int stream_read(void *ptr, size_t size, size_t nmemb, struct cdx_stream_info *stream)
{
	unsigned int req_bytes;
	int read_bytes;
	SftHttpStream *sft_stream;
	if(stream == NULL)
		return -1;
	sft_stream = (SftHttpStream *)stream->data_src_desc.sft_stream_handle;

	req_bytes = size * nmemb;
	if(req_bytes <= 0)
		return -1;

	read_bytes = sft_stream->Read(ptr, req_bytes);
	if(read_bytes < 0)
		read_bytes = 0;
	return read_bytes / size;
}

static int  stream_write(const void *ptr, size_t size, size_t nmemb, struct cdx_stream_info *stream)
{
	int ret = 0;
	return ret;
}

static long long stream_get_size(struct cdx_stream_info *stream)
{
	SftHttpStream *sft_stream;
	if(stream == NULL)
		return -1;

	sft_stream = (SftHttpStream *)stream->data_src_desc.sft_stream_handle;

	return sft_stream->GetSize();
}

//below two function used for m3u/ts
static long long stream_seek_to_time(struct cdx_stream_info *stream, long long us)
{
	int64_t ret = 0;
	return ret;
}

static long long stream_get_total_duration(struct cdx_stream_info *stream)
{
	int64_t ret = 0;
	return ret;
}

static void stream_reset(struct cdx_stream_info *stream)
{
	int ret = 0;
	SftHttpStream *sft_stream;
	if(stream == NULL)
		return ;

	sft_stream = (SftHttpStream *)stream->data_src_desc.sft_stream_handle;
	LOGV("reset stream");
	sft_stream->Reset();

}

static int stream_control(struct cdx_stream_info *stream, void *arg, int cmd)
{
	int ret = 0;
	SftHttpStream *sft_stream;
	if(stream == NULL)
		return -1;

	sft_stream = (SftHttpStream *)stream->data_src_desc.sft_stream_handle;
	switch(cmd)
	{
	case CDX_DMX_CMD_GET_CACHE_STATE:
		return sft_stream->GetCacheState((CDX_CACHE_STATE *)arg);
		break;
	case CDX_DMX_CMD_SET_DURATION:
	{
		int64_t duration;
		duration = *((int64_t *)arg);
		sft_stream->SetDuration(duration);
		break;
	}

	case CDX_DMX_CMD_SET_TRACK_INFO:
	{
		int32_t trackNum = *((int32_t *)arg);
		sft_stream->SetTrackInfo(trackNum);
		break;
	}

	case CDX_DMX_CMD_SNIFF_SFT_MIME:
	{
		ret = sft_stream->Sniff();
		break;
	}

	case CDX_DMX_CMD_SET_DEFAULT_LOW_WATER_THRESHOLD:
		sft_stream->generalInterface(kSetLowwaterThresholdBytes, *(int *)arg, 0);
		break;

	case CDX_DMX_CMD_SET_DEFAULT_HIGH_WATER_THRESHOLD:
		sft_stream->generalInterface(kSetHighwaterThresholdBytes, *(int *)arg, 0);
		break;

	default:
		break;
	}
	return ret;
}

int stream_decrypt(void *ptr, size_t size, int pkt_type, struct cdx_stream_info *stream) {
	SftHttpStream *sft_stream;
	if(stream == NULL)
		return -1;

	sft_stream = (SftHttpStream *)stream->data_src_desc.sft_stream_handle;
	return sft_stream->Decrypt(ptr, size, pkt_type);
}

int sft_http_create_stream_handle(CedarXDataSourceDesc *datasource_desc, struct cdx_stream_info *stream)
{
	int ret;

	SftHttpStream *sft_stream = (SftHttpStream *)datasource_desc->sft_stream_handle;
	LOGV("sft_http_stream url: %s", datasource_desc->source_url);

	if(sft_stream == NULL) {
		sft_stream = new(SftHttpStream);
		datasource_desc->sft_stream_handle 			= sft_stream;
		LOGV("sft_stream_handle %p", datasource_desc->sft_stream_handle);
		ret = sft_stream->Open(datasource_desc);
		if(ret < 0) {
			LOGW("open sft_http_stream failed");
			pthread_mutex_lock(&datasource_desc->sft_handle_mutex);
			//don't switch the order of following lines.
			datasource_desc->sft_stream_handle = NULL;
			delete sft_stream;
			pthread_mutex_unlock(&datasource_desc->sft_handle_mutex);
			return ret;
		}
	}

	stream->seek               = stream_seek;
	stream->tell               = stream_tell;
	stream->read               = stream_read;
	stream->seek_to_time       = stream_seek_to_time;
	stream->get_total_duration = stream_get_total_duration;
	stream->reset_stream       = stream_reset;
	stream->control_stream     = stream_control;
	stream->write 			   = stream_write;
	stream->getsize 		   = stream_get_size;
	stream->decrypt			   = stream_decrypt;

	stream->data_src_desc.sft_stream_handle 	= sft_stream;
	stream_reset(stream);
	datasource_desc->sft_stream_handle_num ++;
	LOGV("datasource_desc->sft_stream_handle_num %d", datasource_desc->sft_stream_handle_num);
	return 0;
}

void sft_http_destory_stream_handle(struct cdx_stream_info *stream)
{
	LOGV("destroy sft_http_stream handle");
	SftHttpStream *sft_stream;
	if(stream == NULL)
		return ;

	sft_stream = (SftHttpStream *)stream->data_src_desc.sft_stream_handle;
	pthread_mutex_lock(&stream->another_data_src_desc->sft_handle_mutex);
	if(sft_stream) {
		sft_stream->Close();
		delete sft_stream;
		sft_stream = NULL;
	}

	LOGV("sft_stream_handle %p", stream->another_data_src_desc->sft_stream_handle);
	stream->data_src_desc.sft_stream_handle = NULL;
	stream->another_data_src_desc->sft_stream_handle = NULL;

	pthread_mutex_unlock(&stream->another_data_src_desc->sft_handle_mutex);
}

void sft_http_force_stop(void * handle)
{
	LOGV("disconnect http");
	if(handle == NULL)
		return ;

	CedarXDataSourceDesc *datasource_desc = (CedarXDataSourceDesc *)handle;
	LOGV("sft_stream_handle %p", datasource_desc->sft_stream_handle);
	pthread_mutex_lock(&datasource_desc->sft_handle_mutex);

	SftHttpStream *sft_stream = (SftHttpStream *)datasource_desc->sft_stream_handle;

	if(sft_stream) {
		sft_stream->Disconnect();
	}
	pthread_mutex_unlock(&datasource_desc->sft_handle_mutex);
}

} //end extern "C"

} //end android

