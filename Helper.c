#include "functions.h"


uint8_t ** create2DArray(uint16_t width, uint16_t height){
   /* allocate memory for a 2d array */
   int i;
   uint8_t **image = (uint8_t **) malloc (height * sizeof(uint8_t *));
   for (i = 0; i < height; i++)
   {
      image[i] = (uint8_t *)malloc (width * sizeof(uint8_t));

      if (image[i] == NULL)
      {
         fprintf(stderr, "Not enough memory to allocate\n");
         exit(1);
      }
   }
   
   return image;
}

float ** readKernel(ArraySize* kSize, char * kernelFile ){
    /* Read Kernel*/
   /* open output file for reading*/
   FILE *fKernel = fopen(kernelFile, "r");
   if (fKernel == NULL)
   {
      fprintf(stderr, "Can't open output file %s!\n", kernelFile);
      exit(1);
   }
   
   int kernelSize = kSize->width; //width = height
   
   float *kernelBuff = (float *) malloc (kernelSize*kernelSize*sizeof(int));
   
   int i, j;
   float **kernel = (float **) malloc (kernelSize * sizeof(float *));
   for (i = 0; i < kernelSize; i++)
   {
      kernel[i] = (float *)malloc (kernelSize * sizeof(float));

      if (kernel[i] == NULL)
      {
         fprintf(stderr, "Not enough memory to allocate\n");
         exit(1);
      }
   }
   
   
   /* populate the 2d array with Kernel information */;
   for (i = 0; i < kernelSize; i++){
       for (j = 0; j < kernelSize; j++){
           float val;
           fscanf(fKernel, "%f", &val);
           kernel[i][j] = val;
           printf("Kernel Count: %f %d %d\n", kernel[i][j],i,j);
       }
   }
   
   /* close the file */
   fclose(fKernel);
   return kernel;

}

float ** createEmptyKernel(int kernelSize){
   int i;
   float **kernel = (float **) malloc (kernelSize * sizeof(float *));
   for (i = 0; i < kernelSize; i++)
   {
      kernel[i] = (float *)malloc (kernelSize * sizeof(float));

      if (kernel[i] == NULL)
      {
         fprintf(stderr, "Not enough memory to allocate\n");
         exit(1);
      }
   }
   return kernel;
}

void printKernel(float **kernel, ArraySize *size){
    int i,j;
 /* populate the 2d array with Kernel information */;
   printf("\n***Printing Kernel***\n");
   for (i = 0; i < size->height; i++){
       for (j = 0; j < size->width; j++){
           float val;
           printf("Kernel Count: %f %d %d\n", kernel[i][j],i,j);
       }
   }

}

float ** flipKernel(float ** kernel, ArraySize * size){
  float ** newKernel = createEmptyKernel(size->width);
  int i,j;
  uint16_t mid = size->width/2;
  //Flip Horizontally
  for(i = 0; i < size->height; i++){
    newKernel[i][mid] = kernel[i][mid];
    for(j = 0; j < mid; j++){
        float temp = kernel[i][j];
        newKernel[i][j] = kernel[i][size->width - 1 - j];
        newKernel[i][size->width - 1 - j] = temp;
    }
  }
  
  printKernel(newKernel,size);
  
  mid = size->height/2;
  //Flip Vertically
  for(i = 0; i < mid; i++){
    for(j = 0; j < size->width; j++){
        float temp = newKernel[i][j];
        newKernel[i][j] = newKernel[size->height - 1 - i][j];
        newKernel[size->height - 1 - i][j] = temp;
    }
  }
  
  printKernel(newKernel,size);
  return newKernel;
}

float ** normalizeKernel(float ** kernel, ArraySize *size){
  float ** newKernel = createEmptyKernel(size->width);
  int i,j;
  float sum = 0;
  for(i = 0; i < size->height; i++){
    for(j = 0; j < size->width; j++){
        //sum += kernel[i][j]*kernel[i][j];
        sum += kernel[i][j];
    }
  }
  
  double sumD = (double)sum;
  float sumRoot = sum;
  //float sumRoot = (float)sqrt(sumD);
  
  for(i = 0; i < size->height; i++){
    for(j = 0; j < size->width; j++){
        newKernel[i][j] = kernel[i][j]/sumRoot;
    }
  }
  printKernel(newKernel,size);
  return newKernel;
}

Point * computeCentroid(uint8_t ** image, uint16_t width, uint16_t height){
   /* Compte the sum I(x,y) */
   int sum = 0;
   int i,j = 0;
   for (i = 0; i < height; i++)
       for (j = 0; j < width; j++)
           sum += image[i][j];
           
   //printf("Sum:%u\n",sum);
           
   /* Allocate Space for Centroid*/
   Point * centroid = (Point*)malloc(sizeof(Point));
   int xSum = 0;
   int ySum = 0;
   /* Compute x and y Value*/
   for (i = 0; i < height; i++){
       for (j = 0; j < width; j++){
           xSum += (i*image[i][j]);
           ySum += (j*image[i][j]);
       }
   }
   
   //printf("X:%u Y:%u\n",xSum, ySum);
   centroid->x = xSum/sum;
   centroid->y = ySum/sum;
   
   
   return centroid;
}


uint8_t ** filterImage(uint8_t ** origImage, uint8_t ** modImage, ArraySize * imageSize, float ** kernel, ArraySize *kernelSize ){
    int i,j,p,q;
    int count = -1;
    for(i = 0; i < imageSize->height; i++){
        for(j = 0; j < imageSize->width; j++){
            int xMinBound = (j-kernelSize->width/2);
            int yMinBound = (i-kernelSize->height/2);
            int sum = 0;
            if(count >= 0){
                printf("\nMain Pixel: i:%d j:%d\n",i,j);
                printf("\np bound: pLB:%d pUB:%d\n",(i-kernelSize->height/2),(i+kernelSize->height/2));
                printf("\nq Pixel: qLB:%d qUB:%d\n",(j-kernelSize->width/2),(j+kernelSize->width/2));
            }
            //Iterate the kernel Array
            for(p = (i-kernelSize->height/2); p <= (i+kernelSize->height/2); p++){
                for(q = (j-kernelSize->width/2); q <= (j+kernelSize->width/2); q++){
                    if(p<0 || q<0 || p>=imageSize->height || q>=imageSize->width){
                        if(count >= 0)
                            printf("\nSkipping p:%d q:%d\n",p,q);
                        continue;
                    }
                    float kernelVal = kernel[p-yMinBound][q-xMinBound];
                    sum += kernelVal*origImage[p][q];
                    
                    if(count >= 0){
                        printf("\n Pixel p:%d q:%d val:%f sum:%d\n",p,q,kernelVal,sum);
                    }
                }
            }
            count --;
            
            if(sum > 255)
                sum = 255;
            if(sum < 0)
                sum = 0;
            modImage[i][j] = sum;
            
        }
    }
    return modImage;
}

void writeImage(uint8_t **image, ArraySize * size, char *fileName){
    uint8_t *buff = (uint8_t *) malloc ((size->width)*(size->height)*sizeof(uint8_t));
   
   /* assign image data to the array */
   int j,i, count = 0;
   for (i = 0; i < size->height; i++)
       for (j = 0; j < size->width; j++)
            buff[count++] = image[i][j];


   /* open output file */
   FILE* fout = fopen(fileName, "wb");
   
   if (fout == NULL)
   {
      fprintf(stderr, "Can't open output file %s!\n", fileName);
      exit(1);
   }


   /* write the header */
   fprintf(fout, "P5\n%u %u\n255\n", size->width, size->height);


   /* write the array */
   fwrite(buff, sizeof(uint8_t), (size->width)*(size->height), fout);


   /* close the file */
   fclose(fout);

}


uint8_t ** readImage(char * fileName, uint8_t **image, uint16_t width, uint16_t height){
   uint8_t *buff = (uint8_t *) malloc (width*height*sizeof(uint8_t));

   /* open output file for reading*/
   FILE *fin = fopen(fileName, "rb");
   if (fin == NULL)
   {
      fprintf(stderr, "Can't open output file %s!\n", fileName);
      exit(1);
   }


   /* skip the first 3 lines */
   char line[255] = {0x0};
   fgets(line, 255, fin);
   //printf("%s\n",line);
   fgets(line, 255, fin);
   //printf("%s\n",line);
   fgets(line, 255, fin);
   //printf("%s\n",line);

   
    /* read the image in one array */
   fread(buff, sizeof(uint8_t), width * height, fin);

   int i;
   
   /* populate the 2d array with image information */
   int j, count = 0;
   for (i = 0; i < height; i++)
       for (j = 0; j < width; j++)
           image[i][j] = buff[count++];
           
    /* close the file */
   fclose(fin);


   /* free memory */
   free(buff);
   
   return image;
}

void freeImage(uint8_t **image, uint16_t height){
   int i;
   for (i = 0; i < height; i++)
      free(image[i]);

   free(image);

}


void processString(char * str){
    int i;
    for(i = 0; i < strlen(str); i++){
        if(str[i] == '.'){
            str[i+4] = '\0';
            break;
        }
    }
}

int findDistance(Point *c1, Point *c2){
    uint16_t x1 = c1->x;
    uint16_t y1 = c1->y;
    
    uint16_t x2 = c2->x;
    uint16_t y2 = c2->y;
    
    return (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2);
}

uint8_t findMaxDiff(uint8_t **image1, uint8_t **image2, ArraySize *size){
   uint8_t maxDiff = 0;
   int i,j = 0;
   for (i = 0; i < size->height; i++){
       for (j = 0; j < size->width; j++){
           uint8_t diff = 0;
           if(image1[i][j] > image2[i][j])
               diff = image1[i][j] - image2[i][j];
           else
               diff = image2[i][j] - image1[i][j];
           if(diff > maxDiff)
               maxDiff = diff;
       }
   }

   return maxDiff;
}
