#ifndef FFMPEG_MEDIAPLAYER_H
#define FFMPEG_MEDIAPLAYER_H

#include <pthread.h>
#include <utils/threads.h>
#include <media/MediaPlayerInterface.h>
#include <utils/List.h>
#include <utils/Vector.h>
#include "vpu_global.h"




extern "C" {
#include "libavcodec/avcodec.h"
#include "libavcodec/get_bits.h"
#include "libavformat/avformat.h"
#include "libavutil/log.h"
#include "libavutil/avutil.h"
#include "libavutil/avstring.h"
#include "libavformat/url.h"
#include "libavformat/avio.h"
} // end of extern C

#include "bluray/include/BlurayManager.h"
#include "bluray/include/BlurayConfig.h"
#include "bluray/include/BlurayCallBack.h"


using namespace RKBluray;



#ifndef INT64_C
#define INT64_C
#define UINT64_C 
#endif



struct DecoderAudio;
struct DecoderVideo;
struct DecoderSubtitle;
struct PacketQueue;
struct FFTimedText;
struct QueueManager;



namespace android {
#ifdef AVS44
struct IGraphicBufferProducer;
#else
struct ISurfaceTexture;
#endif
	struct RkAudioPlayer;
	struct RkFrameManage;
	struct RkRenderer;

#define MEDIA_PLAYER_DECODED 1 << 8

#define BLURAY_VIDEO 0
#define BLURAY_AUDIO 1
#define BLURAY_SUBTITLE 2



/****************************************************************************/


typedef struct RkFFStrmSelect
{
    int32_t aStrmIdx;
    int32_t vStrmIdx;
    int32_t sStrmIdx;

}RkFFStrmSelect_t;

class SeekCondition
{
public:
    int64_t   mStartOffset;
    int64_t   mEndOffset;
    bool      mEnable;

    SeekCondition()
    {
        mEnable = false;
        mStartOffset = 0;
        mEndOffset = 0;
    }
};



class RKBOXFF_MediaPlayer:public BlurayPlayCallBack
{
public:
    RKBOXFF_MediaPlayer();
    virtual ~RKBOXFF_MediaPlayer();

    void setListener(const wp<MediaPlayerBase> &listener);

    status_t setDataSource(int fd, int64_t offset, int64_t length);
    status_t setDataSource(const char *uri, const KeyedVector<String8, String8> *headers);
    
#ifdef AVS44
    status_t setSurfaceTexture(const sp<IGraphicBufferProducer> &bufferProducer);
#else
    status_t setSurfaceTexture(const sp<ISurfaceTexture> &surfaceTexture);
#endif
	status_t bluraySetDataSource();
    void MakeFullHeaders(const KeyedVector<String8, String8> *overrides, String8 *headers);//ht for mutil-headers
    void setAudioSink(const sp<MediaPlayerBase::AudioSink> &audioSink);

    void            checkVideoInfoChange(void* frame,DEC_TYPE dec_type);
    void            initRender(int halFormat,int changeNum);//ht for dynamic rate
	status_t        start();
	status_t        blurayStart();
	status_t        pause();
	bool            isPlaying();
	status_t        getVideoWidth(int *w);
	status_t        getVideoHeight(int *h);
	status_t        seekTo(int64_t timeUs);
	status_t        getDuration(int64_t *durationUs);
	status_t        reset();
    status_t        getPosition(int64_t *positionUs);
	status_t        setAudioStreamType(int type);
	status_t		prepare();
	status_t		blurayPrepare();
    status_t        prepareAsync();
	void            notify(int msg, int ext1, int ext2);
    status_t        invoke(const Parcel &request, Parcel *reply);
    status_t        getParameter(int key, Parcel *reply);
    status_t         setParameter(int key, const Parcel &request);
    uint32_t        flags() const ;

	status_t        resume();

    /////////////////////////////add by hh for bluray///////////////////////
    virtual void play(int type);
    virtual void stopPlay(int type);
    virtual void destory(int type);
    virtual void configVideo(bool configChange,int pid,Vector<BlurayVideoConfig*>* list,int begin);
    virtual void configAudio(bool configChange,int pid,Vector<BlurayAudioConfig*>* list,int begin);
    virtual void configSubtitle(bool configChange,int pid,Vector<BluraySubtitleConfig*>* list,int begin);

    /* ����:����������Ƶ�ļ�����λ�õ�seek
    *  ���÷�Χ: �����û�������seek�����͵���������seek
    *  ����: listPosition: �����б�mVideolist��λ�õ�����
    *        offset: mVideolist[listPosition]��Ӧm2ts��ƫ�ƣ���ֻ��һ������ֵ
    *        time: ��ʼ���ŵ�ʱ��(ms)
    */
    virtual void videoSeek(int listPosition,int64_t offset,int64_t time);

    /* ����:����������Ƶ�ļ�����λ�õ�seek��ֻ�е�����Ƶ����ͬһ���ļ���ʱ���ú�������Ч��������Ƶλ�õ�ƫ��ͬ��Ƶ��ƫ��
    *  ���÷�Χ: �����û�������seek�����͵���������seek
    *  ����: listPosition: �����б�mVideolist��λ�õ�����
    *        offset: mAudiolist[listPosition]��Ӧm2ts��ƫ�ƣ���ֻ��һ������ֵ
    *        time: ��ʼ���ŵ�ʱ��(ms)
    */
    virtual void audioSeek(int listPosition,int64_t offset,int64_t time);

    /* ����:����������Ļ�ļ�����λ�õ�seek��ֻ�е���Ļ����Ƶ����ͬһ���ļ���ʱ���ú�������Ч��������Ļλ�õ�ƫ��ͬ��Ƶ��ƫ��
    *  ���÷�Χ: �����û�������seek�����͵���������seek
    *  ����: listPosition: �����б�mVideolist��λ�õ�����
    *        offset: mSubtitlelist[listPosition]��Ӧm2ts��ƫ�ƣ���ֻ��һ������ֵ
    *        time: ��ʼ���ŵ�ʱ��(ms)
    */
    virtual void subtitleSeek(int listPosition,int64_t offset,int64_t time);

    /* ����: �ı����Text Subtitle��ʽ
    *  ���÷�Χ: �����û������ĸı�Text Subtitle��ʽ���ú���ֻ�е�Text Subtitle��User Styleʱ���ܸı�
    *  ����: style: User Style������
    *  ����ֵ: 0: �ɹ�  1: ʧ��
    */
    virtual int textSubtitleStyleChange(int style);
    virtual int  subtitleOnOff(int on);
    virtual void playButtonSound(int sample,int chanel,int bitPerSample,int length,unsigned char* soundData);
    virtual void sendMsgToPlayer(int type,int code,int reson);
    virtual int seekTimeTo(int msec);
    virtual void sendMsg(int type,int code,int reson);
    virtual void setState(int status);
    
    enum SeekType {
        NO_SEEK,
        SEEK,
        SEEK_VIDEO_ONLY,
        SEEK_DONE,
    };
    
    int64_t         onDisplayEvent();

    void seekAudioIfNecessary();
    void finishSeekIfNecessary(void* avPkt);

    enum FlagMode {
        SET,
        CLEAR,
        ASSIGN
    };
    void modifyFlags(unsigned value, FlagMode mode);
private:
     enum PlayerStatus{
        PLAYER_STATE_ERROR        = 0,
        PLAYER_IDLE               = 0x01,
        PLAYER_INITIALIZED        = 0x02,
        PLAYER_PREPARING          = 0x04,
        PLAYER_PREPARED           = 0x08,
        PLAYER_STARTING           = 0x09,
        PLAYER_STARTED            = 0x10,
        PLAYER_PLAY               = 0x11,
        PLAYER_PAUSED             = 0x20,
        
        PLAYER_STOPPED            = 0x40,
        PLAYER_PLAYBACK_COMPLETE  = 0x80,
        PLAYER_DECODED            = 0x0100,
        PLAYER_READ_END_OF_STREAM = 0x0200,
        PLAYER_PLAY_NEXT_LIST     = 0x0400, // for bluray
    };

    enum BlurayPlayerStatus
    {
        PLAYER_PLAYED_ALL         = 0x0,
        PLAYER_PAUSED_VIDEO       = 0x1,
        PLAYER_PAUSED_AUDIO       = 0x2,
        PLAYER_PAUSED_SUBTITLE    = 0x4,
        PLAYER_PAUSED_ALL         = 0x7,
    };


    enum ReadStatus
    {
        READ_START = 0,
        READ_STOP  = 0X01,
        READ_EXIT  = 0X02,
    };

    enum {
        PLAYING             = 0x01,
        LOOPING             = 0x02,
        FIRST_FRAME         = 0x04,
        PREPARING           = 0x08,
        PREPARED            = 0x10,
        AT_EOS              = 0x20,
        PREPARE_CANCELLED   = 0x40,
        CACHE_UNDERRUN      = 0x80,
        AUDIO_AT_EOS        = 0x0100,
        VIDEO_AT_EOS        = 0x0200,
        AUTO_LOOPING        = 0x0400,

        // We are basically done preparing but are currently buffering
        // sufficient data to begin playback and finish the preparation phase
        // for good.
        PREPARING_CONNECTED = 0x0800,

        // We're triggering a single video event to display the first frame
        // after the seekpoint.
        SEEK_PREVIEW        = 0x1000,

        AUDIO_RUNNING       = 0x2000,
        AUDIOPLAYER_STARTED = 0x4000,

        INCOGNITO           = 0x8000,

        TEXT_RUNNING        = 0x10000,
        TEXTPLAYER_INITIALIZED  = 0x20000,

        SLOW_DECODER_HACK   = 0x40000,
    };


    status_t                setVideoScalingMode(int32_t mode);
    status_t                setVideoScalingMode_l(int32_t mode);
    status_t                selectTrack(size_t trackIndex, bool select);
    status_t                getTrackInfo(Parcel *reply);
    status_t                selectAudioTrack_l(size_t trackIndex);
    status_t                selectSubtitleTrack_l(size_t trackIndex);
	static void				ffmpegNotify(void* ptr, int level, const char* fmt, va_list vl);
//	static void*			startPlay(void* ptr);

	static void 			decode(void* frame, int64_t pts,DEC_TYPE dec_type,void *me);
	static void 			decode(int16_t* buffer, int buffer_size,int64_t pts,int32_t dealt,void *me);
    static void             decodeSubtitle(void* obj, int32_t msg_type, void *me);

	void					decodeMovie(void* ptr);
	int32_t                 getAdtsFrameLength(FILE *fp);         //add by xhr, for aac duration
	int32_t                 getAacDuration(const char *url);      //add by xhr, for aac duration
	void					onBufferingThread(void* ptr); //add by xhr, for buffering thread
	static void*            BufferingPlayer(void* ptr);   //add by xhr, for buffering thread
	status_t                ResetBlurayPlayList(const char *url);//add by xhr, for Bluray, function is to reset bluray play list
    int64_t                 hlsReadBaseTimeUs(bool discontinuity,int64_t currentbase = -1);
    void                    onBufferingUpdate();
    int32_t                 getCacheSize();
    int32_t                 getCacheduration();

    void                    notifyListener_l(int msg, int ext1, int ext2 = 0);
    void                    notifyTimedTextListener(const Parcel *parcel);
    void                    parseTimedText(void* sp, int32_t msg_type);
    void                    freeFFSubPicture(void* sp);
    void                    writeTimedTextToParcel(void* data, int size, int timeMs);
    void                    pushOtherPacket(void *packet);
    void                    deleteOtherPacket();
    void                    checkAC3orDTS(int index);
    status_t                pause_l();
    void                    initAudioPlayer();
    int32_t                 checkVc1StreamKeyFrame(void* stream, void* packet);
    int64_t                 GetNowUs();
    int64_t                 hlsBaseTimeProcess(int64_t pts);

    wp<MediaPlayerBase>     mListener;

    int32_t                     mCurSubFrmDurMs;
    
    int                         mRkHwdecFlag;
    Mutex                       mLock;




	status_t				prepareAudio(AVFormatContext* s, void *ptr);
	status_t				prepareVideo(AVFormatContext* s, void *ptr);
    status_t                prepareSubtitle(AVFormatContext* s, void *ptr);
	static void*    		readVideoPacketThread(void * ptr);
	static void*    		readAudioPacketThread(void * ptr);
	static void*    		readSubtitlePacketThread(void * ptr);
	static void*    		decodePacketThread(void * ptr);
	status_t 				initFrameManger();
	void     				showVersion();                                    //new Construction
	void                    allDestory();                         //������һ�������б�Ķ���
	void                    allInit();                              //��ʼ����ǰ�б�Ķ���
	void                    initAllVariable();                    //��ʼ�����õı���
	bool                    checkAllDecoderReady();                 //�ж����е�decoder�Ƿ��ʼ�����


	DecoderAudio            *mDecoderAudio;                 //��Ƶ���������������
	DecoderVideo            *mDecoderVideo;                 //��Ƶ��������Ӳ������
	DecoderSubtitle         *mDecoderSubtitle;              //��Ļ���������������

    int32_t                 mColorFormat;                    //��Ⱦ��ʽ

    int                     mVideoCreateCount;



public:
	
    RkFrameManage               *pfrmanager;                     //��������Ƶ֡
    RkRenderer                  *mRender;
	sp<ANativeWindow>       	mNativeWindow;
	RkAudioPlayer               *mAudioPlayer;
	
private:
	PlayerStatus                mCurrentState;
    uint32_t          mBlurayState;
	uint32_t mFlags;
	int64_t  mStartedRealTime;
	SeekType mSeeking;
    int64_t mSeekTimeUs;
    int64_t mPreSeekTimeUs;
    int64_t mPreSeekSysTimeUs;
	int64_t mVideoTimeUs;
	int64_t mVideoPreTimeUs;
  	int64_t mVideoLastTimeUs;
	
	



/*******add for test, function***********/
status_t				readVideoPacket();
status_t				readAudioPacket();
status_t				readSubtitlePacket();
bool                    isComplete(AVFormatContext *ic);
bool                    checkConsumeAll();
bool                    controlCacheSize(); //control whether cache size is too large
void                    stopDecoder();      //stop decoder work
void                    deleteDecoder();    //delete decoder work
void                    pauseDecoder();    //pause decoder work
void                    flushAVPlayerCache(); //flush rkframemanager and audiaplayer cache
void                    seekOperation();
void                    playListPlayComplete();
void                    playNextFileInPlayList();



void  prepareDecode(void *ptr);
void  pushPacketToDecoder(AVPacket* packet,  AVFormatContext* s);

void  videoDestory();
void  audioDestory();
void  subtitleDestory();









	


/********add for test********/
	BlurayManager*              mBlurayManager;
	QueueManager				*mDemuxerPacket;				 //�����������������ݶ���
	Vector<BlurayVideoConfig*>* 		mVideolist;
	Vector<BlurayAudioConfig*>* 		mAudiolist;
	Vector<BluraySubtitleConfig*>* 	    mSubtitlelist;
	sp<MediaPlayerBase::AudioSink> mAudioSink;
	
	int                                 mVideoPosition;
	int                                 mAudioPosition;
    int                                 mSubtitlePosition;

    SeekCondition                       mVideoSeekCondition;
    SeekCondition                       mAudioSeekCondition;
    SeekCondition                       mSubtitleSeekCondition;
    

	
	AVFormatContext *                   mVideoFormatContext;
    AVFormatContext *                   mAudioFormatContext;
    AVFormatContext *                   mSubtitleFormatContext;
	

	RkFFStrmSelect_t            mSelect;                         //��������Ƶ����Ļѡ���track


	pthread_t                   mReadVideoThread;                   //��������Ƶ�������̱߳�ʾ
	pthread_t                   mReadAudioThread;                   //��������Ƶ�������̱߳�ʾ
	pthread_t                   mReadSubtitleThread;                   //��������Ļ�������̱߳�ʾ

    int                         mVideoThreadStatus;                 // ����Video Packet Read �߳��Ƿ񴴽�
    int                         mAudioThreadStatus;                 // ����Audio Packet Read �߳��Ƿ񴴽�
    int                         mSubtitleThreadStatus;              // ����Subtitle Packet Read �߳��Ƿ񴴽�

	int                         mVideoWidth;                     //��Ƶ�Ŀ�
    int                         mVideoHeight;					 //��Ƶ�ĸ�

    ReadStatus                        mVideoReadStatus;
    ReadStatus                        mAudioReadStatus;
    ReadStatus                        mSubtitleReadStatus;


	FILE *mFd;
    bool mIsBlurayReady;
    bool isHlsFlag;
    bool mCanSeek;

    String8                   mFilePath;
    String8                   mExtension;
    
    int32_t                   mContinueDisCard;
    /*
    * ���õĲ��������Ƿ��Ѿ�������ϣ�ĳЩiso(�ӷ�è)�ĵ�������û����Ƶ������仯�ǳ�Ƶ��
    * ���¼������ť�������õ����������������������
    */
    bool                      mPlayListPlay;
	
    RKBOXFF_MediaPlayer(const RKBOXFF_MediaPlayer &);
    RKBOXFF_MediaPlayer &operator=(const RKBOXFF_MediaPlayer &);

    //////////////////////////////////////////bluray/////////////////////////////////////////////////////
	status_t 				initFFmpegVideoDecoder(AVFormatContext *ic);
	status_t 				initFFmpegAudioDecoder(AVFormatContext *ic);
	status_t 				initFFmpegSubtitleDecoder(AVFormatContext *ic);
	
	status_t                initVideoContext();
    status_t                initAudioContext();
    status_t                initSubtitleContext();
    status_t                initVideoTrack(AVFormatContext *ic);
    status_t                initAudioTrack(AVFormatContext *ic);
    status_t                initSubtitleTrack(AVFormatContext *ic);
    
    int getAudioTrack(Parcel *reply);
    int getSutitleTrack(Parcel *reply);
};
}
#endif // FFMPEG_MEDIAPLAYER_H
