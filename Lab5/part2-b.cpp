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

/* 
src= 
    shared memory: https://www.geeksforgeeks.org/ipc-shared-memory/    
    semaphore across processes: http://www.vishalchovatiya.com/semaphore-between-processes-example-in-c/
*/
using namespace std;


struct rgbpix{
    int r,g,b;
};
/* global semaphore variable */
const char *semName = "asdfsd";

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
        // vector<rgbpix> row;
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
    FILE *rfp = fopen("3p3.ppm","r");
    FILE *wfp = fopen("3p3_op.ppm","w");
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
   
    int SHMSIZE=sizeof(rgbpix);
    size_t bytes = (sizeof(data[0][0]) * data[0].size())*data.size();
    //Creating a child process
    int pid = fork();
    if(pid == 0){ //child process
        sem_t *sem = sem_open(semName, O_CREAT, 0600, 0);
        invert(ref(data));
        key_t key = 1234;
        int shmid;
        char *shm, *s;
        // shmget returns an identifier in shmid
        if((shmid= shmget(key,bytes,0666|IPC_CREAT))<0){
            perror("shmget");
            printf("shmget failed\n");
            return 0;
        }
        
        // shmat to attach to shared memory
        if((shm = (char*)shmat(shmid,(void*)NULL,0))==(char*)-1){
            perror("shmat");
            printf("shmat failed\n");
            return 0;
        }
        s=shm;
        for(i=0;i<height;i++){
            for(j=0;j<width;j++){
                int r,g,b;
                struct rgbpix pix;
                pix=data[i][j];
                r=pix.r;
                g=pix.g;
                b=pix.b;
                string str=to_string(r)+" "+to_string(g)+" "+to_string(b)+"\n";
                for(int k=0;k<str.length();k++){
                    *s++ = str[k]; 
                }
            }
        }
        shmdt(shm);
        printf("Child done\n"); 
        sem_post(sem); //After the child process is done, it will post the semaphore, so that parent can proceed
        return 0;
    }

    else{ //parent process
        sem_t *sem = sem_open(semName, O_CREAT, 0600, 0);
        sem_wait(sem);     //Wait for child to finish, and run only after post
        key_t key = 1234;
        int shmid;
        char *shm, *s;
        // shmget returns an identifier in shmid
        if((shmid= shmget(key,bytes,0666))<0){
            perror("shmget");
            printf("shmget failed\n");
            return 0;
        }
        // shmat to attach to shared memory
        if((shm = (char*)shmat(shmid,(void*)NULL,0))==(char*)-1){
            perror("shmat");
            printf("shmat failed\n");
            return 0;
        }
        s=shm;
        int ct=0;
        for(i=0;i<height;i++){
            for(j=0;j<width;j++){
                struct rgbpix pix;
                string temp_r="", temp_g="", temp_b="";
                while(s[ct]!=' ' && s[ct]!='\n'){
                    temp_r+=s[ct];
                    ct++;
                }
                ct++; //skip space
                while(s[ct]!=' ' && s[ct]!='\n'){
                    temp_g+=s[ct];
                    ct++;
                }
                ct++; //skip space
                while(s[ct]!=' ' && s[ct]!='\n'){
                    temp_b+=s[ct];
                    ct++;
                }
                ct++; //skip space
                int r=stoi(temp_r);
                pix.r=r;
                int g=stoi(temp_g);
                pix.g=g;
                int b=stoi(temp_b);
                pix.b=b;
                data[i][j]=pix;
            }
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