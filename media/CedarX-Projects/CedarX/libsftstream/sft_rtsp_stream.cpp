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
#define LOG_TAG "sft_rtsp_stream"
#include <CDX_Debug.h>

#include <CDX_Common.h>
#include <cedarx_stream.h>
#include <cedarx_demux.h>

#include <HTTPBase.h>
#include <media/stagefright/foundation/ABase.h>
#include <media/stagefright/foundation/AHandlerReflector.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/foundation/ABuffer.h>
#include <media/stagefright/MediaErrors.h>
#include <RTSPSource.h>

namespace android {

struct BufferCache {
	BufferCache();
    ~BufferCache();

    struct Page {
        void *mData;
        size_t mSize;
        Page(void *data, size_t size) {
        	mData = malloc(size);
        	mSize = size;
        	CHECK(mData != NULL);
        	memcpy(mData, data, size);
        }
    };

    void releasePage(Page *page);

    void appendPage(Page *page);
    size_t releaseFromStart(size_t maxBytes);

    size_t totalSize() const {
        return mTotalSize;
    }

    void copy(size_t from, void *data, size_t size);

private:
    size_t mTotalSize;

    List<Page *> mValidPages;

    void freePages(List<Page *> *list);

    DISALLOW_EVIL_CONSTRUCTORS(BufferCache);
};

BufferCache::BufferCache()
	:mTotalSize(0){

}

BufferCache::~BufferCache() {
	freePages(&mValidPages);
}

void BufferCache::freePages(List<Page *> *list) {
	List<Page *>::iterator it = list->begin();
	while(it != list->end()) {
		Page * page = *it;
		free(page->mData);
		delete page;
		page = NULL;
		++it;
	}
}

void BufferCache::releasePage(Page *page)
{
	free(page->mData);
	delete page;
	page = NULL;
}

void BufferCache::appendPage(Page *page)
{
//	LOGV("mTotalSize %u, page size %u", mTotalSize, page->mSize);
	mTotalSize += page->mSize;
	mValidPages.push_back(page);
}


size_t BufferCache::releaseFromStart(size_t maxBytes) {
    size_t bytesReleased = 0;

    while (maxBytes > 0 && !mValidPages.empty()) {
        List<Page *>::iterator it = mValidPages.begin();

        Page *page = *it;

        if (maxBytes < page->mSize) {
            break;
        }

        mValidPages.erase(it);

        maxBytes -= page->mSize;
        bytesReleased += page->mSize;

        releasePage(page);
    }
    mTotalSize -= bytesReleased;
    LOGV("mTotalSize %u, bytesReleased %u", mTotalSize, bytesReleased);
    return bytesReleased;
}

void BufferCache::copy(size_t from, void *data, size_t size) {
//    LOGV("copy from %d size %d", from, size);

    if (size == 0) {
        return;
    }

    CHECK_LE(from + size, mTotalSize);

    size_t offset = 0;
    List<Page *>::iterator it = mValidPages.begin();
    while (from >= offset + (*it)->mSize) {
        offset += (*it)->mSize;
        ++it;
    }

    size_t delta = from - offset;
    size_t avail = (*it)->mSize - delta;

    if (avail >= size) {
        memcpy(data, (const uint8_t *)(*it)->mData + delta, size);
        return;
    }

    memcpy(data, (const uint8_t *)(*it)->mData + delta, avail);
    ++it;
    data = (uint8_t *)data + avail;
    size -= avail;

    while (size > 0) {
        size_t copy = (*it)->mSize;
        if (copy > size) {
            copy = size;
        }
        memcpy(data, (*it)->mData, copy);
        data = (uint8_t *)data + copy;
        size -= copy;
        ++it;
    }
}

//#define SAVE_STREAM_DATA
class SftRtspStream:public RefBase{
public:
	SftRtspStream();
	virtual ~SftRtspStream();

	virtual int Open(CedarXDataSourceDesc *datasrc_desc);
	virtual int Close();
	virtual int Read(void *ptr, size_t size);
	virtual int GetCacheState(CDX_CACHE_STATE *cdx_cache_state);
	virtual int SeekToPos(long long offset, int whence);
	virtual void SetDuration(int64_t duration);
	virtual int64_t Tell();
	virtual int64_t GetSize();
	virtual void Reset();
	virtual status_t SeekToTime(int64_t seekTimeUs);
    void Disconnect();

private:
	friend struct AHandlerReflector<SftRtspStream>;
    enum {
        kWhatFetchMore  = 'fetc',
        kWhatRead       = 'read',
        kWhatSeek		= 'seek',
    };
    enum {
        kDefaultCacheThreshold      = 2 * 1024 * 1024,
        kDefaultFlushThreshold      = 1 * 1024 * 1024,
        kDefaultReadTimeoutUs		= 2 * 60 *1000*1000,
    };

	bool mUIDValid;
	uid_t mUID;
	int64_t mFileLen;
	int64_t mBitrate;
	int64_t mDurationUs;
#ifdef SAVE_STREAM_DATA
	int fd;
#endif
	int64_t mLastAccessPos;
	status_t mFinalStatus;
	String8 mUri;
	sp<RTSPSource> mRtspSource;
    sp<AHandlerReflector<SftRtspStream> > mReflector;
    sp<ALooper> mLooper;
    KeyedVector<String8, String8> mUriHeaders;

    BufferCache *mCache;
	int64_t mCacheOffset;
	Mutex mLock;
    Condition mReadCondition;
    Condition mSeekCondition;
    sp<AMessage> mAsyncResult;
    bool mForceDisconnect;
	void onMessageReceived(const sp<AMessage> &msg);
    void onFetch();
    void onRead(const sp<AMessage> &msg);
    size_t approxDataRemaining_l(status_t *finalStatus);
};

SftRtspStream::SftRtspStream():
		mFileLen(0),
		mBitrate(25000),
		mDurationUs(0),
		mLastAccessPos(0),
		mFinalStatus(OK),
	    mReflector(new AHandlerReflector<SftRtspStream>(this)),
	    mLooper(new ALooper),
	    mCache(new BufferCache),
	    mCacheOffset(0),
	    mForceDisconnect(false) {

#ifdef SAVE_STREAM_DATA
	fd = open("/data/camera/parser.dat", O_CREAT | O_WRONLY);
	if(fd < 0) {
		LOGD("open file failed,");
	}
#endif
	mUriHeaders.clear();

}

SftRtspStream::~SftRtspStream()
{
#ifdef SAVE_STREAM_DATA
	if(fd >= 0) {
		close(fd);
	}
#endif
	delete mCache;
	mCache = NULL;
}

int SftRtspStream::Open(CedarXDataSourceDesc *datasrc_desc)
{
	sp<RefBase> obj = (RefBase *)datasrc_desc->sft_rtsp_source;
	mRtspSource = static_cast<RTSPSource *>(obj.get());
	if(mRtspSource == NULL) {
		return -1;
	}
	mRtspSource->getDuration(&mDurationUs);

	mLooper->setName("sft_rtsp_stream");
	mLooper->registerHandler(mReflector);
    mLooper->start();
    Mutex::Autolock autoLock(mLock);
    (new AMessage(kWhatFetchMore, mReflector->id()))->post();

	return 0;
}

int SftRtspStream::Close()
{
    mLooper->stop();
    mLooper->unregisterHandler(mReflector->id());

    mFinalStatus = ERROR_END_OF_STREAM;
	mReadCondition.broadcast();
	mSeekCondition.broadcast();

	return 0;
}


void SftRtspStream::onMessageReceived(const sp<AMessage> &msg) {
    switch (msg->what()) {
        case kWhatFetchMore:
        {
            onFetch();
            break;
        }

        case kWhatRead:
        {
            onRead(msg);
            break;
        }

        default:
            TRESPASS();
    }
}

void SftRtspStream::onFetch()
{
    sp<ABuffer> accessUnit;
    int64_t  mediaTimeUs;
    status_t ret;
    int64_t delayUs = 0;
    bool skipData = false;
    //TODO: only dequeue unit of index 0
    //maybe error.
    status_t err = mRtspSource->dequeueAccessUnit(0, &accessUnit);
//    LOGV("err %d, mRtspSource %p", err, mRtspSource.get());
    if (err == -EWOULDBLOCK) {
    	if ((ret = mRtspSource->feedMoreTSData()) != OK) {
    		LOGW("feedMoreTSData fail");
    		mFinalStatus = ret;
    		mSeekCondition.signal();
    		return ;
    	}
    	delayUs = 50*1000;
    } else if (err != OK) {
        if (err == INFO_DISCONTINUITY) {
        	//TODO:add someting here
        	LOGH;
            size_t totalSize = mCache->totalSize();
            CHECK_EQ(mCache->releaseFromStart(totalSize), totalSize);
        } else if(err == INFO_FORMAT_CHANGED) {
        	//TODO:add someting here
        	skipData = true;
        	LOGI("accessUnit->size() %d", accessUnit->size());
        } else {
        	mFinalStatus = err;
        	LOGI("final status %d", mFinalStatus);
        	mReadCondition.broadcast();
        	mSeekCondition.broadcast();
        	return ;
        }
    }
    if(accessUnit.get() != NULL &&
    		(accessUnit->size() > 0) && !skipData) {
        Mutex::Autolock autoLock(mLock);
    	BufferCache::Page *page = new BufferCache::Page((void *)(accessUnit->data()), accessUnit->size());
    	mCache->appendPage(page);
    	mSeekCondition.signal();
    }
    (new AMessage(kWhatFetchMore, mReflector->id()))->post(delayUs);
}

void SftRtspStream::onRead(const sp<AMessage> &msg)
{
    size_t size;
    CHECK(msg->findSize("size", &size));

    status_t finalStatus;
    size_t remain = approxDataRemaining_l(&finalStatus);

    if((finalStatus == OK) && (remain < size)) {
    	msg->post(50000);
    	return;
    }

    CHECK(mAsyncResult == NULL);
    mAsyncResult = new AMessage;
    remain = remain < size ? remain : size;
    mAsyncResult->setSize("result", remain);

    mReadCondition.signal();
}

size_t SftRtspStream::approxDataRemaining_l(status_t *finalStatus)
{
    *finalStatus = mFinalStatus;

    Mutex::Autolock autoLock(mLock);
    off64_t lastBytePosCached = mCacheOffset + mCache->totalSize();
    if (mLastAccessPos < lastBytePosCached) {
        return lastBytePosCached - mLastAccessPos;
    }
    return 0;
}

//functions to operate stream
int SftRtspStream::Read(void * ptr, size_t size)
{
	ssize_t readBytes = 0;

	if(mFileLen > 0 && mLastAccessPos > mFileLen) {
		LOGW("read beyond file");
		return -1;
	}

	Mutex::Autolock autoLock(mLock);

	size_t maxBytes = mLastAccessPos - mCacheOffset;
	//keep 1M data remained.
	maxBytes -= (maxBytes > kDefaultFlushThreshold) ? kDefaultFlushThreshold : 0;
	if(maxBytes > kDefaultFlushThreshold) {
		LOGV("release from start. mLastAccessPos %lld, mCacheOffset %lld, bytes %u",
				mLastAccessPos, mCacheOffset, maxBytes);
		size_t actualBytes = mCache->releaseFromStart(maxBytes);
		mCacheOffset += actualBytes;
	}

    //read data.
    size_t delta = mLastAccessPos - mCacheOffset;
    size_t avail = mCache->totalSize() - delta;
    if(mFinalStatus == OK) {
    	if(mLastAccessPos < mCacheOffset
    			|| size > avail) {
    		LOGV("Beyond cache. size %u, mLastAccessPos %lld, mCacheOffset %lld, cache size %lld", size,
    		   		mLastAccessPos, mCacheOffset ,(int64_t)(mCache->totalSize()));
    		sp<AMessage> msg = new AMessage(kWhatRead, mReflector->id());

    		msg->setSize("size", size);
    	    msg->post();
    	    CHECK(mAsyncResult == NULL);

    	    while(mAsyncResult == NULL) {

    	    	if(mFinalStatus != OK) {
    	    		LOGI("stream end ? return.");
    	    		mAsyncResult.clear();
    	    		return -1;
    	    	}
    	    	mReadCondition.wait(mLock);
    	    }
    	    mAsyncResult->findSize("result", &size);
    	    mAsyncResult.clear();
    	    LOGV("request finish");
    	}
    } else {
    	size = size > avail ? avail : size;
    	LOGV("size %u", size);
    }

    mCache->copy(delta, ptr, size);
    mLastAccessPos += size;
    readBytes = size;

#ifdef SAVE_STREAM_DATA
	if(fd >= 0) {
		write(fd, ptr, readBytes);
	}
#endif
//	LOGV("read over, read bytes %ld", readBytes);
	return readBytes;
}
status_t SftRtspStream::SeekToTime(int64_t seekTimeUs)
{
	if(seekTimeUs < 0 || seekTimeUs > mDurationUs) {
		return -1;
	}

	Mutex::Autolock autoLock(mLock);

    mRtspSource->seekTo(seekTimeUs);

    size_t totalSize = mCache->totalSize();
    CHECK_EQ(mCache->releaseFromStart(totalSize), totalSize);
    mLastAccessPos = 0;
    mCacheOffset = 0;
    mFinalStatus = OK;
	return OK;
}

int SftRtspStream::SeekToPos(long long offset, int whence)
{
	int ret = 0;

	if(mFinalStatus != OK) {
		return -1;
	}

	if(whence == SEEK_CUR) {
		mLastAccessPos += offset;
	} else if(whence == SEEK_SET) {
		mLastAccessPos = offset;
	} else if(whence == SEEK_END) {
		mLastAccessPos = mFileLen + offset;
	} else {
		ret = -1;
	}

	if(mLastAccessPos < 0)
		mLastAccessPos = 0;

	//seek beyond end of file
	if((mFileLen > 0) && (mLastAccessPos > mFileLen)) {
		mLastAccessPos = mFileLen + 1;
		ret = -1;
	}
	Mutex::Autolock autoLock(mLock);
	//if seeking pos is beyond cached size, maybe error.
	if(mLastAccessPos < mCacheOffset) {
		LOGV("seek. file pos %lld, mCacheOffset %lld,  cache size %u, whence %d",
				mLastAccessPos, mCacheOffset, mCache->totalSize() ,whence);
		TRESPASS();
	}
	while(mLastAccessPos > (int64_t)(mCacheOffset + mCache->totalSize())) {
		LOGV("seek. file pos %lld, mCacheOffset %lld,  cache size %u, whence %d",
				mLastAccessPos, mCacheOffset, mCache->totalSize() ,whence);
#if 1
		if(mFinalStatus != OK) {
			break;
		}
	// 	wait for data enough
		mSeekCondition.wait(mLock);
#else
	//for test.
	TRESPASS();
#endif
	}

	return ret;
}

int64_t SftRtspStream::Tell()
{
//	LOGV("tell....Are you kidding me???");
	return mLastAccessPos;
}

int64_t SftRtspStream::GetSize()
{
	return 0;
}

void SftRtspStream::Reset()
{
	mLastAccessPos  = 0;
	mCacheOffset 	= 0;
}

void SftRtspStream::SetDuration(int64_t duration)
{
	LOGV("set duration %lld", duration);
	mDurationUs = duration;
	mBitrate 	= mFileLen * 8000000ll / mDurationUs;
}

int SftRtspStream::GetCacheState(CDX_CACHE_STATE *cdx_cache_state)
{
	status_t finalStatus;
	size_t remainData = approxDataRemaining_l(&finalStatus);
	if(finalStatus != OK) {
		cdx_cache_state->filled_percent = 100;
		cdx_cache_state->eof_flag = 1;
	} else {
		cdx_cache_state->filled_percent = remainData * 100/kDefaultCacheThreshold;
	}
	cdx_cache_state->filled_size = remainData;
//	LOGV("cdx_cache_state->filled_percent %d, remainData %u",
//		cdx_cache_state->filled_percent, remainData);
	return OK;
}

void SftRtspStream::Disconnect()
{
	mForceDisconnect = true;
    mFinalStatus = ERROR_END_OF_STREAM;
	mReadCondition.broadcast();
	mSeekCondition.broadcast();
}

extern "C" {

static int stream_seek(struct cdx_stream_info *stream, cdx_off_t offset, int whence)
{
	if(stream == NULL)
		return -1;

	sp<RefBase> obj = (RefBase *)stream->data_src_desc.sft_stream_handle;
	sp<SftRtspStream> sft_stream = static_cast<SftRtspStream *>(obj.get());

	return sft_stream->SeekToPos(offset, whence);
}

static cdx_off_t stream_tell(struct cdx_stream_info *stream)
{
	if(stream == NULL)
		return -1;

	sp<RefBase> obj = (RefBase *)stream->data_src_desc.sft_stream_handle;
	sp<SftRtspStream> sft_stream = static_cast<SftRtspStream *>(obj.get());

	return sft_stream->Tell();
}

static int stream_read(void *ptr, size_t size, size_t nmemb, struct cdx_stream_info *stream)
{
	unsigned int req_bytes;
	int read_bytes;

	if(stream == NULL)
		return -1;

	sp<RefBase> obj = (RefBase *)stream->data_src_desc.sft_stream_handle;
	sp<SftRtspStream> sft_stream = static_cast<SftRtspStream *>(obj.get());

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
	if(stream == NULL)
		return -1;

	sp<RefBase> obj = (RefBase *)stream->data_src_desc.sft_stream_handle;
	sp<SftRtspStream> sft_stream = static_cast<SftRtspStream *>(obj.get());

	return sft_stream->GetSize();
}

//below two function used for m3u/ts
static long long stream_seek_to_time(struct cdx_stream_info *stream, long long us)
{
	if(stream == NULL)
		return -1;

	sp<RefBase> obj = (RefBase *)stream->data_src_desc.sft_stream_handle;
	sp<SftRtspStream> sft_stream = static_cast<SftRtspStream *>(obj.get());

	return sft_stream->SeekToTime(us);
}

static long long stream_get_total_duration(struct cdx_stream_info *stream)
{
	int64_t ret = 0;
	return ret;
}

static void stream_reset(struct cdx_stream_info *stream)
{
	int ret = 0;

	if(stream == NULL)
		return ;

	sp<RefBase> obj = (RefBase *)stream->data_src_desc.sft_stream_handle;
	sp<SftRtspStream> sft_stream = static_cast<SftRtspStream *>(obj.get());

	sft_stream->Reset();

}

static int stream_control(struct cdx_stream_info *stream, void *arg, int cmd)
{
	int ret = 0;
	if(stream == NULL)
		return -1;

	sp<RefBase> obj = (RefBase *)stream->data_src_desc.sft_stream_handle;
	sp<SftRtspStream> sft_stream = static_cast<SftRtspStream *>(obj.get());
	if(sft_stream == NULL) {
		return -1;
	}

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

	default:
		break;
	}
	return ret;
}

int sft_rtsp_create_stream_handle(CedarXDataSourceDesc *datasource_desc, struct cdx_stream_info *stream)
{
	int ret;
	LOGV("sft_rtsp_stream url: %s", datasource_desc->source_url);

	sp<SftRtspStream> sft_stream = new SftRtspStream;
	datasource_desc->sft_stream_handle 			= sft_stream.get();
	ret = sft_stream->Open(datasource_desc);
	if(ret < 0) {
		pthread_mutex_lock(&datasource_desc->sft_handle_mutex);
		LOGW("open sft_rtsp_stream failed");
		datasource_desc->sft_stream_handle = NULL;
		sft_stream.clear();
		pthread_mutex_unlock(&datasource_desc->sft_handle_mutex);
		return ret;
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

	sft_stream->incStrong(NULL);
	stream->data_src_desc.sft_stream_handle 	= sft_stream.get();
	return 0;
}

void sft_rtsp_destory_stream_handle(struct cdx_stream_info *stream)
{
	LOGV("destroy sft_rtsp_stream handle");
	if(stream == NULL)
		return ;

	sp<RefBase> obj = (RefBase *)stream->data_src_desc.sft_stream_handle;
	sp<SftRtspStream> sft_stream = static_cast<SftRtspStream *>(obj.get());
	pthread_mutex_lock(&stream->another_data_src_desc->sft_handle_mutex);
	if(sft_stream != NULL) {
		sft_stream->decStrong(NULL);
		sft_stream->Close();
		sft_stream.clear();
	}
	stream->data_src_desc.sft_stream_handle = NULL;
	stream->another_data_src_desc->sft_stream_handle = NULL;
	pthread_mutex_unlock(&stream->another_data_src_desc->sft_handle_mutex);
}

void sft_rtsp_stop(void * handle)
{
	LOGV("disconnect rtsp");
	if(handle == NULL)
		return ;

	CedarXDataSourceDesc *datasource_desc = (CedarXDataSourceDesc *)handle;
	LOGV("sft_stream_handle %p", datasource_desc->sft_stream_handle);
	pthread_mutex_lock(&datasource_desc->sft_handle_mutex);

	sp<RefBase> obj = (RefBase *)datasource_desc->sft_stream_handle;
	sp<SftRtspStream> sft_stream = static_cast<SftRtspStream *>(obj.get());

	if(sft_stream != NULL) {
		sft_stream->Disconnect();
	}
	pthread_mutex_unlock(&datasource_desc->sft_handle_mutex);
}

} //end extern "C"

} //end android

