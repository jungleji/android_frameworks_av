/*
* Rockchip Bluray
* Meta Data parse
* author:hh@rock-chips.com
*/

#ifndef _RK_BLURAY_META_H_
#define _RK_BLURAY_META_H_


#include <stdio.h>

#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include <libxml/tree.h>


namespace RKBluray 
{


#define MAX_META_FILE_SIZE  0xfffff
#define Meta_MAX  100


class MetaThumbnail
{
public:
    char *                   path;
    unsigned int             width;
    unsigned int             hegiht;

    MetaThumbnail()
    {
        path = NULL;
        width = hegiht = 0;
    }

    ~MetaThumbnail()
    {
        if(path != NULL)
        {
            delete[] path;
            path = NULL;
        }
    }
} ;
    
class MetaTitle 
{
public:
    unsigned int   mNumber;
    char *         mName;
public:
    MetaTitle()
    {
        mName = NULL;
        mNumber = 0;
    }

    ~MetaTitle()
    {
        if(mName != NULL)
        {
            delete[] mName;
            mName = NULL;
        }
    }
} ;
    
class  MetaDL {
public:
    int                  mThumbCount;
    int                  mNumSet;                 // Disc Count
    int                  mSetNumber;              // Current Disc NO.
    unsigned int         mTableOfCount;
    char                 language_code[4];        //  this meta language, Meta may have some xml
    char *               filename;                //  paser xml's name
    char *               mDiscName;                // Disc Name
    char *               di_alternative;
    MetaTitle *          mTableOfContents[Meta_MAX];
    MetaThumbnail *      thumbnails[Meta_MAX];

    MetaDL()
    {
        mThumbCount = 0;
        mNumSet = 0;
        mSetNumber = 0;
        mTableOfCount = 0;

        filename = NULL;
        mDiscName = NULL;
        di_alternative = NULL;

        for(int i = 0; i < Meta_MAX; i++)
        {
            mTableOfContents[i] = NULL;
            thumbnails[i] = NULL;
        }
    }

    ~MetaDL()
    {
        mThumbCount = 0;
        mNumSet = 0;
        mSetNumber = 0;
        mTableOfCount = 0;

        if(filename != NULL)
        {
            delete[] filename;
            filename = NULL;
        }

        if(mDiscName != NULL)
        {
            delete[] mDiscName;
            mDiscName = NULL;
        }

        if(di_alternative != NULL)
        {
            delete[] di_alternative;
            di_alternative = NULL;
        }

        for(int i = 0; i < Meta_MAX; i++)
        {
            if(mTableOfContents[i] != NULL)
            {
                delete mTableOfContents[i];
            }

            if(thumbnails[i] != NULL)
            {
                delete thumbnails[i];
            }
        }
    }
} ;

class Meta
{
private:
    int  mCount;
    char* mPath;
    MetaDL* mEntries[Meta_MAX];

private:    
    int findMetaXMLfiles();
    int parseManifestNode(xmlNodePtr node, MetaDL *disclib);
public:
    Meta(char* path);
    ~Meta();
    int parse();
    int getCount();
    MetaDL* getEntry(int index);
};

}

#endif
