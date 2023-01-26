#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "version.h"
//#define putcode(a,b) putc(a,b)
using namespace std;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
enum CODE_ID
{
    NOP,END,RETS,RETL,
    PUSHa,PUSHreg,PUSHimm8,PUSHimm16,PUSHimm32,PUSHimm64,
    POPa,POPreg,
    absJMPimm64,absJMPreg,absJMPregreg,absJMPregimm64,
    JMPimm64,JMPreg
};
enum TYPE_INFO_SIZE {imm,mem,rmm0,rmm1,reg};
struct TYPE
{
    TYPE_INFO_SIZE TYP;
    u64 IMM;
    u16 REG;
};

void getcode(void);
TYPE getnum(void);
void putcode(CODE_ID a,FILE*b){
    fwrite(&a,2,1,b);
}
void help (void);
void error_(void);
FILE*fip,*fop;
unsigned long LINECNT;


int main (int argc, char**argv)
{
    if((argc!=3)||(argv[1][0]=='?'))
    {
        help();
        return 1;
    }
    fip=fopen(argv[1],"r");
    fop=fopen(argv[2],"wb");
    LINECNT=0;
    while(!feof(fip))
    {
        LINECNT++;
        getcode();
    }
    fclose(fip),fclose(fop);
    return 0;
}

void getcode(void)
{
    static char buff[1024];
    fscanf(fip,"%s",buff);
    strlwr(buff);
    if(buff[0]==';')
        while(getc(fip)=='\n');
    else if(!strcmp(buff,"rem"))
        fscanf(fip,"%*s");
    else if(!strcmp(buff,"nop"))
        putcode(NOP,fop);
    else if(!strcmp(buff,"end"))
        putcode(END,fop);
    else if(!strcmp(buff,"ret"))
        putcode(RETS,fop);
    else if(!strcmp(buff,"rets"))
        putcode(RETS,fop);
    else if(!strcmp(buff,"retl"))
        putcode(RETL,fop);
    else if(!strcmp(buff,"popa"))
        putcode(POPa,fop);
    else if(!strcmp(buff,"pusha"))
        putcode(PUSHa,fop);
    else if(!strcmp(buff,"push"))
    {
        TYPE ttyp=getnum();
        u64 tmp=ttyp.IMM;
        if(ttyp.TYP==reg)
        {
            putcode(PUSHreg,fop);
            fwrite(&ttyp.REG,2,1,fop);
        }
        else if(ttyp.TYP==imm)
        {
            putcode(PUSHimm64,fop);
            fwrite(&tmp,sizeof(tmp),1,fop);
        }
        else error_();
    }
    else if(!strcmp(buff,"pushb"))
    {
        TYPE ttyp=getnum();
        u8 tmp=ttyp.IMM;
        if(ttyp.TYP==imm)
        {
            putcode(PUSHimm8,fop);
            fwrite(&tmp,sizeof(tmp),1,fop);
        }
        else error_();
    }
    else if(!strcmp(buff,"pushw"))
    {
        TYPE ttyp=getnum();
        u16 tmp=ttyp.IMM;
        if(ttyp.TYP==imm)
        {
            putcode(PUSHimm16,fop);
            fwrite(&tmp,sizeof(tmp),1,fop);
        }
        else error_();
    }
    else if(!strcmp(buff,"pushd"))
    {
        TYPE ttyp=getnum();
        u32 tmp=ttyp.IMM;
        if(ttyp.TYP==imm)
        {
            putcode(PUSHimm32,fop);
            fwrite(&tmp,sizeof(tmp),1,fop);
        }
        else error_();
    }
    else if(!strcmp(buff,"pushq"))
    {
        TYPE ttyp=getnum();
        u64 tmp=ttyp.IMM;
        if(ttyp.TYP==imm)
        {
            putcode(PUSHimm64,fop);
            fwrite(&tmp,sizeof(tmp),1,fop);
        }
        else error_();
    }
    else if(!strcmp(buff,"pop"))
    {
        TYPE ttyp=getnum();
        if(ttyp.TYP==reg)
        {
            putcode(POPreg,fop);
            fwrite(&ttyp.REG,2,1,fop);
        }
        else error_();
    }
    else if(!strcmp(buff,"jmp"))
    {
        TYPE ttyp=getnum();
        u64 tmp=ttyp.IMM;
        if(ttyp.TYP==reg)
        {
            putcode(absJMPreg,fop);
            fwrite(&ttyp.REG,2,1,fop);
        }
        else if(ttyp.TYP==imm)
        {
            putcode(absJMPimm64,fop);
            fwrite(&tmp,sizeof(tmp),1,fop);
        }
        else error_();
    }
    else if(!strcmp(buff,"jmps"))
    {
        TYPE ttyp=getnum();
        u64 tmp=ttyp.IMM;
        if(ttyp.TYP==reg)
        {
            putcode(JMPreg,fop);
            fwrite(&ttyp.REG,2,1,fop);
        }
        else if(ttyp.TYP==imm)
        {
            putcode(JMPimm64,fop);
            fwrite(&tmp,sizeof(tmp),1,fop);
        }
        else error_();
    }
    else if(!strcmp(buff,"jmpl"))
    {
        TYPE ttyp=getnum();
        u64 tmp=ttyp.IMM;
        if(ttyp.TYP==reg)
        {
            putcode(absJMPregreg,fop);
            fwrite(&ttyp.REG,2,1,fop);
        }
        else if(ttyp.TYP==imm)
        {
            putcode(absJMPregimm64,fop);
            fwrite(&tmp,sizeof(tmp),1,fop);
        }
        else error_();
    }
    else error_();
}

TYPE getnum (void)
{
    char type;
    TYPE ttyp;
    while((type=getc(fip))==' '||type=='\t'||type==0||type=='\n'||type=='\r');
    if(type=='h')
    {
        fscanf(fip,"%llx",&ttyp.IMM);
        ttyp.TYP=imm;
    }
    else if(type=='o')
    {
        fscanf(fip,"%llo",&ttyp.IMM);
        ttyp.TYP=imm;
    }
    else if(type=='b')
    {
        ttyp.IMM=0,ttyp.TYP=imm;
        while(!feof(fip))
        {
            type=getc(fip);
            ttyp.IMM<<=1;
            if(type=='1')ttyp.IMM++;
            else if(type=='0');
            else
            {
                ttyp.IMM>>=1;
                break;
            }
        }
        ttyp.TYP=imm;
        ungetc(type,fip);
    }
    else if((type>='0'&&type<='9')||(type=='+')||(type=='-'))
    {
        ungetc(type,fip),ttyp.TYP=imm;
        fscanf(fip,"%llu",&ttyp.IMM);
    }
    else if(type=='$')
    {
        while((type=getc(fip))==' '||type=='\t'||type==0||type=='\n'||type=='\r');
        if(type=='h')
            fscanf(fip,"%hx",&ttyp.REG);
        else if(type=='o')
            fscanf(fip,"%ho",&ttyp.REG);
        else if(type=='b')
        {
            ttyp.REG=0;

            while(!feof(fip))
            {
                type=getc(fip);
                ttyp.IMM<<=1;
                if(type=='1')ttyp.IMM++;
                else if(type=='0');
                else
                {
                    ttyp.IMM>>=1;
                    break;
                }
            }
            ungetc(type,fip);
        }
        else if(type>='0'&&type<='9')
            ungetc(type,fip),fscanf(fip,"%hu",&ttyp.REG);
        else error_();
        ttyp.TYP=reg;
    }
    else if(type=='#')
    {
        while((type=getc(fip))==' '||type=='\t'||type==0||type=='\n'||type=='\r');
        if(type=='h')
        {
            fscanf(fip,"%llx",&ttyp.IMM);
            ttyp.TYP=imm;
        }
        else if(type=='o')
        {
            fscanf(fip,"%llo",&ttyp.IMM);
            ttyp.TYP=imm;
        }
        else if(type=='b')
        {
            ttyp.IMM=0,ttyp.TYP=imm;

            while(!feof(fip))
            {
                type=getc(fip);
                ttyp.IMM<<=1;
                if(type=='1')ttyp.IMM++;
                else if(type=='0');
                else
                {
                    ttyp.IMM>>=1;
                    break;
                }
            }
            ungetc(type,fip);
        }
        else error_();
        while((type=getc(fip))==' '||type=='\t'||type==0||type=='\n'||type=='\r');
        if(type=='-')
        {
            ttyp.TYP=rmm0,getc(fip);
            while((type=getc(fip))==' '||type=='\t'||type==0||type=='\n'||type=='\r');
        }
        else if(type=='+')
        {
            ttyp.TYP=rmm1;
            while((type=getc(fip))==' '||type=='\t'||type==0||type=='\n'||type=='\r');
        }
        else if(type=='$')
            ttyp.TYP=rmm1;
        else error_();
        while((type=getc(fip))==' '||type=='\t'||type==0||type=='\n'||type=='\r');
        if(type=='h')
            fscanf(fip,"%hx",&ttyp.REG);
        else if(type=='o')
            fscanf(fip,"%ho",&ttyp.REG);
        else if(type=='b')
        {
            ttyp.REG=0;

            while(!feof(fip))
            {
                type=getc(fip);
                ttyp.IMM<<=1;
                if(type=='1')ttyp.IMM++;
                else if(type=='0');
                else
                {
                    ttyp.IMM>>=1;
                    break;
                }
            }
            putc(type,fip);
        }
        else if(type>='0'&&type<='9')
            ungetc(type,fip),fscanf(fip,"%hu",&ttyp.REG);
        else error_();
    }
    return ttyp;
}

void help (void)
{
    static char DOC[][1024]= {"SymLang Compiler \n\tProduced by Symbol\n\nSYMC InputFile OutputFile\n\
    If InputFile is \'?\', then you\'ll see this. \n\tOr you enter not 2 values to the program, you\'ll see this too.\n\n",
                              "SymLang Compiler Version: "
                             };

    strcat(DOC[1],AutoVersion::STATUS);
    strcat(DOC[1]," ");
    strcat(DOC[1],AutoVersion::FULLVERSION_STRING);

    printf(DOC[0]);
    printf(DOC[1]);
    return;
}
void error_(void)
{
    printf("ERROR at:%lu",LINECNT);
    exit(2);
    return;
}
