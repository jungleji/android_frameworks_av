/*
*this file define by hehua to parse Navigation Command in MovieObject.bdmv when playing bluRay disc.
*/


#ifndef NAVIGATIONCMDPARSE_H_
#define NAVIGATIONCMDPARSE_H_


#include "NavigationCommand.h"
#include "BlurayRegisters.h"
#include "BlurayPlayController.h"
//#include "Bluray.h"


namespace RKBluray {

/* nav cmd defines */
/* groups */
#define GROUP_BRANCH    0
#define GROUP_COMPARE   1
#define GROUP_SET       2

/* subgroups */
#define SUBGROUP_GOTO   0
#define SUBGROUP_JUMP   1
#define SUBGROUP_PLAY   2

/* branch group, goto subgoup */
#define BR_NOP      0
#define BR_GOTO     1
#define BR_BREAK    2

/* branch group, jump subgoup */
#define BR_JUMPOBJ      0
#define BR_JUMPTITLE    1
#define BR_CALLOBJ      2
#define BR_CALLTITLE    3
#define BR_RESUME       4

/* branch group, play subgoup */
#define BR_PLAYPL   0
#define BR_PLatPI   1
#define BR_PLatMK   2
#define BR_TERMPL   3
#define BR_LINKPI   4
#define BR_LINKMK   5

/* compare group */
#define CM_BC       1
#define CM_EQ       2
#define CM_NE       3
#define CM_GE       4
#define CM_GT       5
#define CM_LE       6
#define CM_LT       7

/* set cmd option */
#define SET_GRP     0
#define SET_SYSTEM  1

/* set command group */
#define SET_MOVE    1
#define SET_SWAP    2
#define SET_ADD     3
#define SET_SUB     4
#define SET_MUL     5
#define SET_DIV     6
#define SET_MOD     7
#define SET_RND     8
#define SET_AND     9
#define SET_OR      10
#define SET_XOR     11
#define SET_BITSET  12
#define SET_BITCLR  13
#define SET_SHL     14
#define SET_SHR     15

/* set system command */
#define SET_STREAM  1
#define SET_NVTIMER 2
#define SET_BTNPG   3
#define SET_ENBTN   4
#define SET_DISBTN  5
#define SET_SECSTM  6
#define SET_POPOFF  7
#define SET_STLON   8
#define SET_STLOFF  9
#define SET_OUTPUT_MODE  10
#define SET_STREAM_SS  11
#define SET_SYSTEM_0x10  0x10



/* Local constants */
#define NAVCMD_OPERAND_UNUSED       0
#define NAVCMD_OPERAND_REG          1
#define NAVCMD_OPERAND_IMM          2
#define NAVCMD_OPERAND_INVALID      3
#define NAVCMD_REGISTER_PSR         1
#define NAVCMD_REGISTER_GPR         2

#define  CMDPROC_SUCCESS            (0)
#define  CMDPROC_FAILURE            (-1)
#define  CMDPROC_NOT_IMPLEMENTED    (-2)
#define  CMDPROC_NULL_POINTER       (-3)
#define  CMDPROC_JUMP_OBJECT        (1)
#define  CMDPROC_CALL_OBJECT        (2)
#define  CMDPROC_JUMP_TITLE         (3)
#define  CMDPROC_CALL_TITLE         (4)
#define  CMDPROC_RESUME_OBJECT      (5)
#define  CMDPROC_WAITING            (6)
#define  CMDPROC_PL_TERMINATED      (7)

typedef enum tagCMDPROC_PRGM_STATE
{
    CMDPROC_STATE_INVALID = 0,
    CMDPROC_STATE_TERMINATED,
    CMDPROC_STATE_MOVIE_PROCESSING,
    CMDPROC_STATE_BUTTON_PROCESSING,
    CMDPROC_STATE_WAITING
} CMDPROC_STATE;



/**
 * Navigation Command Types
 */
typedef enum tagNAVCMD_TYPE

{
    NAVCMD_NOP = 0,
    NAVCMD_GOTO,
    NAVCMD_BREAK,
    NAVCMD_JUMP_OBJECT,
    NAVCMD_JUMP_TITLE,  
    NAVCMD_CALL_OBJECT, //5
    NAVCMD_CALL_TITLE,
    NAVCMD_RESUME,
    NAVCMD_PLAY_PL,
    NAVCMD_PLAY_PLATPI,
    NAVCMD_PLAY_PLATMK,  //10
    NAVCMD_TERMINATE_PL,
    NAVCMD_LINK_PI,
    NAVCMD_LINK_MK,
    NAVCMD_STILL_ON,
    NAVCMD_STILL_OFF,  // 15
    NAVCMD_BC,
    NAVCMD_EQ,
    NAVCMD_NE,
    NAVCMD_GE,
    NAVCMD_GT,  // 20
    NAVCMD_LE,
    NAVCMD_LT,
    NAVCMD_MOVE,
    NAVCMD_SWAP,
    NAVCMD_ADD,  // 25
    NAVCMD_SUB,
    NAVCMD_MULT,
    NAVCMD_DIV,
    NAVCMD_MOD,
    NAVCMD_RND, // 30
    NAVCMD_AND,
    NAVCMD_OR,
    NAVCMD_XOR,
    NAVCMD_BIT_SET,
    NAVCMD_BIT_CLEAR, // 35
    NAVCMD_SHIFT_LEFT,
    NAVCMD_SHIFT_RIGHT,
    NAVCMD_SET_STREAM,
    NAVCMD_SET_NV_TIMER,
    NAVCMD_SET_BUTTON_PAGE, // 40
    NAVCMD_ENABLE_BUTTON,
    NAVCMD_DISABLE_BUTTON,
    NAVCMD_SET_SECONDARY_STREAM,
    NAVCMD_POPUP_MENU_OFF,
    /*2013.9.11 add by hh*/
    NAVCMD_SET_OUTPUT_MODE,  // 45
    NAVCMD_SET_STREAM_SS,
    NAVCMD_SET_STREAM_0x10,

#ifdef DRM_BDPLUS_SUPPORT
    NAVCMD_SET_MESSAGE_BDPLUS,
    NAVCMD_SET_STATUS_BDPLUS,
#endif

    /* All new commands above this line */
    NAVCMD_INVALID
}NAVCMD_TYPE;

class CMDInfor
{
public:
    NavigationCommand* mCmd;
    int mSize;
    int mCurrentCmdId;
public:
    CMDInfor();
};


class NavigationCmdParser
{
    public:
        NavigationCmdParser(BlurayRegisters* reg);
        ~NavigationCmdParser();
        void setLog(bool debug);
        bool isTitleRepeatOn();
        int getNavigationCmdId();
        NAVCMD_TYPE getCommandType(NavigationCommand *pCommand);
        int toString(NavigationCommand *Cmd);
        int parseDstOperandType(unsigned long lOpcode);
        int parseSrcOperandType(unsigned long lOpcode);
        int getMovieCmdId();
        int executeCMD(CMDInfor* infor,unsigned long *paras);
        int doCommand(CMDInfor* infor,int type ,unsigned long *paras);
        int terminateMovieProgram();
        int suspendMovieProgram(int *cmdID);
        int loadMovieProgram(NavigationCommand*cmd,int size,int currentId);
        int playbackCompelete(unsigned long* param);
        int getNavigationTimerJumpObject(unsigned long *movieObjID);
        int setRepeatOn(bool on);
        int processButtonProgram(NavigationCommand *cmd, int size, unsigned long *statusParam);
        int processMovieProgram(NavigationCommand*cmd,int size,unsigned long* param);
        int resumeMovieProgram(NavigationCommand*cmd, int size,int cmdID, unsigned long *param);

        void setPlayController(BlurayPlayController* play);
    private:
        int parseRegisterType(unsigned long ulOperand);
        // function below is navigation command define by Blu-Ray specs
        int Nop();
        int GoTo(CMDInfor* infor);//(NavigationCommand *cmd);
        int Break(CMDInfor* infor, unsigned long *paras);
        int JumpObject(CMDInfor* infor, unsigned long *paras);
        int JumpTitle(CMDInfor* infor,unsigned long *paras);
        int CallObject(CMDInfor* infor, unsigned long *paras);
        int CallTitle(CMDInfor* infor, unsigned long *paras);
        int Resume();

        int playPlayList(CMDInfor* infor, unsigned long *paras);
        int playPLPlayItem(CMDInfor* infor, unsigned long *paras);
        int PlayPLMark(CMDInfor* infor, unsigned long *paras);
        int playLinkPlayItem(CMDInfor* infor, unsigned long *paras);
        int playLinkMark(CMDInfor* infor, unsigned long *paras);
        int TerminatePL(CMDInfor* infor, unsigned long *paras);
        
        /* Compare Command Group*/
        
        // Binary Compare
        int BC(CMDInfor* infor);
        // Equals
        int EQ(CMDInfor* infor);
        // Not Equals
        int NE(CMDInfor* infor);
        // Greater Than or Equal to
        int GE(CMDInfor* infor);
        // Greater Than
        int GT(CMDInfor* infor);
        // Less Than or Equals to
        int LE(CMDInfor* infor);
        // Less Than
        int LT(CMDInfor* infor);

        /* Set Command Group Command*/
        // Set value to GPR
        int Move(CMDInfor* infor);
        // Change values between two GPRs
        int Swap(CMDInfor* infor);
        // Add value to GPR
        int Add(CMDInfor* infor);
        // Subtract value from GPR
        int Sub(CMDInfor* infor);
        // Multiply value to GPR
        int Mul(CMDInfor* infor);
        // Divide value to GPR
        int Div(CMDInfor* infor);
        // Modulus value to GPR
        int Mod(CMDInfor* infor);
        // Store Random number in GPR
        int Rand(CMDInfor* infor);
        // Binary AND to GPR
        int AND(CMDInfor* infor);
        // Binary OR to GPR
        int OR(CMDInfor* infor);
        // Binary XOR to GPR
        int XOR(CMDInfor* infor);
        // Set bit in GPR
        int SetBit(CMDInfor* infor);
        // Set bit to "0" in GPR
        int BitClear(CMDInfor* infor);
        // Zeros are shifted into low order bit in GPR
        int ShiftLeft(CMDInfor* infor);
        // Zeros are shifted into high order bit in GPR
        int ShiftRight(CMDInfor* infor);

        // Set Audio, PG, Text subtitle, Angle and IG streams
        int SetStream(CMDInfor* infor);
        // Set Navigation Timer
        int SetNVTimer(CMDInfor* infor);
        // Set Button ID and/or Page ID
        int setButton(CMDInfor* infor);
        // Set Button to ¡°normal¡± state
        int EnableButton(CMDInfor* infor);
        // Set Button to ¡°normal¡± state
        int DisableButton(CMDInfor* infor);
        // Set Secondary Video, Secondary Audio and PiP PG/Text subtitle streams
        int SetSecondaryStream(CMDInfor* infor);
        // Turn off pop up menu
        int PopUpMenuOff(CMDInfor* infor);
        // Hold playback
        int StillOn(CMDInfor* infor);
        
        int StillOff(CMDInfor* infor);
        // SET SYSTEM 3D mode
        int SetOutputMode(CMDInfor* infor);

        int SetStreamSS(CMDInfor* infor);
        
        int SetStream_0x10(CMDInfor* infor);

        BlurayRegisters* mRegister;
        CMDInfor* mMovieProgram;
        CMDInfor* mButtonProgram;

        CMDPROC_STATE mState;
        bool mTransitionPending;
        bool mTitleRepeatOn;
        unsigned long mNavigationTime;

        BlurayPlayController* mPlayCtrl;

        bool   mDebug;

        pthread_mutex_t mMutex;
};


}
#endif
