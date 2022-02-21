#include<iostream>
#include<vector>
#include <thread>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>

using namespace std;

struct rgbpix{
    int r,g,b;
};

struct pixdata{
    int r,g,b,i,j;
};

/* global variables */
int height=0, width=0, max_val=0;
vector<vector<rgbpix>> data;

//function to covert image to grayscale
//source:https://www.dynamsoft.com/blog/insights/image-processing/image-processing-101-color-space-conversion/
void grayscale(vector<vector<rgbpix>>& arg_data){  
    printf("GrayScale Conversion Started\n");
    int i,j;
    for(i=0;i<height;i++)
    {   
        for(j=0;j<width;j++)
        {
            int r,g,b;
            r=arg_data[i][j].r;
            g=arg_data[i][j].g;
            b=arg_data[i][j].b;
            int avg=(r+g+b)/3;
            arg_data[i][j].r=avg;
            arg_data[i][j].b=avg;
            arg_data[i][j].g=avg;
        }
    }
    printf("GrayScale Conversion Finished\n");
}

// function to invert the color for image
// source: https://www.homeandlearn.co.uk/extras/image/image-invert-colors.html#:~:text=Colour%20inversion%20is%20the%20colour,is%200%2C%200%2C%200
void invert(vector<vector<rgbpix>>& arg_data){ 
    printf("Invert Conversion Started\n");
    int i,j;
    for(i=0;i<height;i++)
    {   
        for(j=0;j<width;j++)
        {
            int r,g,b;
            r=arg_data[i][j].r;
            g=arg_data[i][j].g;
            b=arg_data[i][j].b;
            arg_data[i][j].r=max_val-r;
            arg_data[i][j].b=max_val-b;
            arg_data[i][j].g=max_val-g;
        }
    }
    printf("Invert Conversion Finished\n");
}

int main(int argc, char const *argv[])
{
    clock_t start=clock();
    FILE *rfp = fopen(argv[1],"r");
    FILE *wfp = fopen("output.ppm","w");
    char ppm_version[5];
    fscanf(rfp,"%s",ppm_version);
    if(ppm_version[0]!='P' || ppm_version[1]!='3')
    {
        cout<<"PPM file format not supported!!!"<<endl;
        return 0;
    }

    fscanf(rfp,"%d %d",&width,&height);
    fscanf(rfp,"%d",&max_val);

    int i,j;
    //Reading pixel values from input file
    for(i=0;i<height;i++)
    {
        vector<rgbpix> row;
        for(j=0;j<width;j++)
        {
            int r,g,b;
            struct rgbpix pix;
            fscanf(rfp,"%d %d %d",&r,&g,&b);
            pix.r=r;
            pix.g=g;
            pix.b=b;
            row.push_back(pix);
        }
        data.push_back(row);
    }
    fclose(rfp); //input_data stored in data
    printf("input PPM file read Finished\n");
    int fd_c[2]; //pipe for child process 
    pipe(fd_c);
    //Creating a child
    int pid = fork();
    if(pid == 0){ //child process
        close(fd_c[0]); //close read end of pipe
        invert(ref(data));
        printf("Child Process doing\n");
        for(i=0;i<height;i++){
            for(j=0;j<width;j++){
                int r,g,b;
                struct rgbpix pix;
                pix=data[i][j];
                r=pix.r;
                g=pix.g;
                b=pix.b;
                struct pixdata pixdata;
                pixdata.r=r;
                pixdata.g=g;
                pixdata.b=b;
                pixdata.i=i;
                pixdata.j=j;
                write(fd_c[1],&pixdata,sizeof(pixdata));
            }
        }
        printf("Child done\n"); 
        return 0;
    }

    else{ //parent process
        //pipe from child writes and parent reads
        close(fd_c[1]); //close write end of pipe
        int ct=0;
        struct pixdata pixdata;
        while(ct<height*width){
            read(fd_c[0],&pixdata,sizeof(pixdata));
            data[pixdata.i][pixdata.j].r=pixdata.r;
            data[pixdata.i][pixdata.j].g=pixdata.g;
            data[pixdata.i][pixdata.j].b=pixdata.b;
            ct++;
        }
        grayscale(ref(data));
        printf("Parent done\n");
        fprintf(wfp,"%s\n",ppm_version);
        fprintf(wfp,"%d %d\n",width,height);
        fprintf(wfp,"%d\n",max_val);
        for(i=0;i<height;i++){
            for(j=0;j<width;j++){
                int r,g,b;
                struct rgbpix pix;
                pix=data[i][j];
                r=pix.r;
                g=pix.g;
                b=pix.b;
                fprintf(wfp,"%d %d %d\n",r,g,b);
            }
        }
        fclose(wfp);
        printf("output PPM file write Finished\n");
        printf("Total Time Taken: %f\n",(double)(clock()-start)/CLOCKS_PER_SEC);
        return 0;
    } 
}