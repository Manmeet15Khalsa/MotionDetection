#include <stdio.h>
#include <stdlib.h>
#include "functions.h"

int main(void)
{
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    
    /*
        0 => No Motion
        1 => Motion
    */
    uint8_t centroidFlag = 0;
    uint8_t diffFlag = 0;
    
    int diffThreshold = 100;
    int centroidThreshold = 1;
    ArraySize * smoothSize = (ArraySize *)malloc(sizeof(ArraySize));
    smoothSize->width = 5;
    smoothSize->height = 5;
    float ** smoothKernel = readKernel(smoothSize, "smooth2.txt"); 
    float ** corrKernel = normalizeKernel(smoothKernel, smoothSize);
    
    float ** convKernel = flipKernel(corrKernel, smoothSize);
    
     int i,kernelSize = 3;
  
     ArraySize * kSize = (ArraySize *)malloc(sizeof(ArraySize));
     kSize->width = kernelSize;
     kSize->height = kernelSize;
     
     float ** vKernel = readKernel(kSize, "vkernel.txt");
     float ** hKernel = readKernel(kSize, "hkernel.txt");
    
    fp = fopen("images.txt", "r");
    FILE *fout = fopen("CentroidsDistance.txt", "w");
    if (fp == NULL)
        exit(-1);
     
     /* define fout dimensions */
     uint16_t width   =  320;
     uint16_t height  =  240;
        
     ArraySize * imageSize = (ArraySize *)malloc(sizeof(ArraySize));
     imageSize->width = width;
     imageSize->height = height;
    Point * c1 = NULL;
    Point * c2 = NULL;
    
    uint8_t **array1 = create2DArray(imageSize->width, imageSize->height);
    uint8_t **array2 = create2DArray(imageSize->width, imageSize->height);
    uint8_t **array3 = create2DArray(imageSize->width, imageSize->height);
    
    int firstImage = 1;
    char fileName[100];
    char vName[100];
    char hName[100];
    char corrName[100];
    char convName[100];
    
    //Buffers for motions
    char centroidMotion[100];
    char diffMotion[100];
    
    while ((read = getline(&line, &len, fp)) != -1) {
        processString(line);
        printf("%s\n", line);
        
        //Empty String Name
        bzero(fileName,100);
        bzero(vName,100);
        bzero(hName,100);
        bzero(corrName,100);
        bzero(convName,100);
        
        //Create FileNames
        strcat(fileName,"images/");
        strcat(vName,"images/vEdge-");
        strcat(hName,"images/hEdge-");
        strcat(corrName,"images/corr-");
        strcat(convName,"images/conv-");
        
        strcat(fileName,line);
        strcat(vName,line);
        strcat(hName,line);
        strcat(corrName,line);
        strcat(convName,line);
        
        //Read Image in array1
        array1 = readImage(fileName, array1, width,height);
        
        //Store Modified Image in array2
        array2 = filterImage(array1, array2, imageSize, vKernel, kSize);
        writeImage(array2, imageSize, vName);
            
        //Store Modified Image in array2
        array2 = filterImage(array1, array2, imageSize, hKernel, kSize);
        writeImage(array2, imageSize, hName);
            
        //Store Modified Image in array2
        array2 = filterImage(array1, array2, imageSize, corrKernel, smoothSize);
        writeImage(array2, imageSize, corrName);
            
        //Store Modified Image in array2
        array2 = filterImage(array1, array2, imageSize, convKernel, smoothSize);
        writeImage(array2, imageSize, convName);
            
            
            
        uint8_t **im = NULL;
        if(firstImage == 1){

            c1 = (Point *)computeCentroid(array1, width, height);
            firstImage = 0;
            array3 = readImage(fileName, array3, width,height);
            fprintf(fout,"%s \t (%d , %d) \t %s \t %s \n",line,c1->x,c1->y,"No Motion Centroid","No Motion Diff");
            continue;
        }
        
        //Detecting Motion Using Centroid Method
        c2 = computeCentroid(array1, width, height);
        int distance = findDistance(c1,c2);
        if(distance >= centroidThreshold){
            centroidFlag = 1;
            strcpy(centroidMotion,"Motion Centroid");
        }else{
            //This also handles the case when flag was true earlier
            centroidFlag = 0;
            strcpy(centroidMotion,"No Motion Centroid");
        }
        
        //Detecting Motion Using Max Difference Method
        uint8_t maxDiff = findMaxDiff(array1, array3, imageSize);
        if(maxDiff >= diffThreshold){
            diffFlag = 1;
            strcpy(diffMotion,"Motion Difference");
        }else{
            if(diffFlag == 1){
                diffFlag = 0;
            }
            strcpy(diffMotion,"No Motion Difference");
        }
        
        fprintf(fout,"%s \t (%d , %d) \t %s \t %s\n",line,c1->x,c1->y,centroidMotion,diffMotion);
        c1 = c2;
        array3 = readImage(fileName, array3, width,height);

    }

    fclose(fp);
    if (line)
        free(line);
    return 0;
}
