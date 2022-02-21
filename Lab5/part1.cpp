#include<iostream>
#include<vector>
using namespace std;

struct rgbpix{
    int r,g,b;
};
//function to covert image to grayscale
//source:https://www.dynamsoft.com/blog/insights/image-processing/image-processing-101-color-space-conversion/
void grayscale(vector<vector<rgbpix>>& input_data, vector<vector<rgbpix>>& output_data, int width, int height, int max){  
    int i,j;
    for(i=0;i<height;i++)
    {   
        vector<rgbpix> row;
        for(j=0;j<width;j++)
        {
            int r,g,b;
            struct rgbpix pix;
            r=input_data[i][j].r;
            g=input_data[i][j].g;
            b=input_data[i][j].b;
            int avg=(r+g+b)/3;
            pix.r=avg;
            pix.g=avg;
            pix.b=avg;
            row.push_back(pix);
        }
        output_data.push_back(row);
    }
}
//function to invert the color for image
//source: https://www.homeandlearn.co.uk/extras/image/image-invert-colors.html#:~:text=Colour%20inversion%20is%20the%20colour,is%200%2C%200%2C%200
void invert(vector<vector<rgbpix>>& input_data, vector<vector<rgbpix>>& output_data,int width, int height, int max){  
    int i,j;
    for(i=0;i<height;i++)
    {
        vector<rgbpix> row;
        for(j=0;j<width;j++)
        {
            int r,g,b;
            struct rgbpix pix;
            r=input_data[i][j].r;
            g=input_data[i][j].g;
            b=input_data[i][j].b;
            pix.r=max-r;
            pix.g=max-g;
            pix.b=max-b;
            row.push_back(pix);
        }
        output_data.push_back(row);
    }
}
int main(int argc, char const *argv[])
{
    clock_t start=clock();
    FILE *rfp = fopen("1p3.ppm","r");
    FILE *wfp = fopen("1p3_op.ppm","w");
    int height=0, width=0, max=0;
    char ppm_version[5];
    fscanf(rfp,"%s",ppm_version);
    if(ppm_version[0]!='P' || ppm_version[1]!='3')
    {
        cout<<"PPM file format not supported!!!"<<endl;
        return 0;
    }
    fscanf(rfp,"%d %d",&width,&height);
    fscanf(rfp,"%d",&max);
    fprintf(wfp,"%s\n",ppm_version);
    fprintf(wfp,"%d %d\n",width,height);
    fprintf(wfp,"%d\n",max);
    int i,j;
    vector<vector<rgbpix>> input_data;
    vector<vector<rgbpix>> buffer;
    vector<vector<rgbpix>> output_data;
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
        input_data.push_back(row);
    }
    fclose(rfp);

    grayscale(input_data,buffer,width,height,max);
    invert(buffer,output_data,width,height,max);
    //Writing pixel values to output file
    for(i=0;i<height;i++)
    {
        for(j=0;j<width;j++)
        {
            int r,g,b;
            struct rgbpix pix;
            pix=output_data[i][j];
            r=pix.r;
            g=pix.g;
            b=pix.b;
            fprintf(wfp,"%d %d %d\n",r,g,b);
        }
    }
    fclose(wfp);
    printf("Total Time Taken: %f\n",(double)(clock()-start)/CLOCKS_PER_SEC);
    return 0;
}