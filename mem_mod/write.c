#include <stdio.h>
#include <string.h>

int main(int argc,char** argv)
{
    FILE *fp0 = NULL;
    char Buf[1024];
    memset(Buf,0,1024);
    
    if(argc<=1){
	printf("need input\n");
	return -1;
    }
    strcpy(Buf,argv[1]);
    
    fp0 = fopen("/dev/memdev0","r+");
    if (fp0 == NULL)
    {
        printf("Open Memdev0 Error!\n");
        return -1;
    }
    
    //写入memdev0
    fwrite(Buf, sizeof(Buf), 1, fp0);
    
    fclose(fp0);
    printf("write to mem_dev: %s\n",Buf);
    
    return 0;    

}
