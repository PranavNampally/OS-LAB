#include<iostream>
#include<vector>
#include <thread>
#include <semaphore.h>
/* src= https://www.bogotobogo.com/cplusplus/multithreading_pthread.php */
using namespace std;

struct rgbpix{
    int r,g,b;
};
/* global semaphore variable */
sem_t sem;

/* global variables */
int height=0, width=0, max_val=0;
vector<vector<rgbpix>> data;

//function to covert image to grayscale
//source:https://www.dynamsoft.com/blog/insights/image-processing/image-processing-101-color-space-conversion/
void grayscale(vector<vector<rgbpix>>& arg_data){  
    sem_wait(&sem); //Acts as Lock
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
    sem_post(&sem); //acts as unlock
}

// function to invert the color for image
// source: https://www.homeandlearn.co.uk/extras/image/image-invert-colors.html#:~:text=Colour%20inversion%20is%20the%20colour,is%200%2C%200%2C%200
void invert(vector<vector<rgbpix>>& arg_data){ 
    sem_wait(&sem); //Acts as Lock
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
    sem_post(&sem); //Acts as Unlock
}

int main(int argc, char const *argv[])
{
    clock_t start=  clock();
    FILE *rfp = fopen(argv[1],"r");
    FILE *wfp = fopen("output_part2_1b.ppm","w");
    char ppm_version[5];
    fscanf(rfp,"%s",ppm_version);
    if(ppm_version[0]!='P' || ppm_version[1]!='3')
    {
        cout<<"PPM file format not supported!!!"<<endl;
        return 0;
    }
    sem_init(&sem, 0, 1); //initialize semaphore to 1 

    fscanf(rfp,"%d %d",&width,&height);
    fscanf(rfp,"%d",&max_val);
    fprintf(wfp,"%s\n",ppm_version);
    fprintf(wfp,"%d %d\n",width,height);
    fprintf(wfp,"%d\n",max_val);

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

    thread T2, T1;
    T1 = thread(invert, ref(data));
    T1.join();
    T2 = thread(grayscale, ref(data));
    T2.join();

    //Writing pixel values to output file
    for(i=0;i<height;i++)
    {
        for(j=0;j<width;j++)
        {
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
    sem_destroy(&sem);
    printf("output PPM file write Finished\n");
    printf("Total Time Taken: %f\n",(double)(clock()-start)/CLOCKS_PER_SEC);
    return 0;
}