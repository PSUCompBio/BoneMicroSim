#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

//functions
int find_minmax(int);
int create_load_curves(int, int, int, int, int);
int write_nodesets(int,int,int,int, int);
int boundary_conditions(int, int, int, int, int);
int read_nodesandelements(int);
int write_nodeselements(int,int,int,int,int);
int write_master(int,int,int,int,int);
int write_submitfiles(int,int,int,int,int);
int write_postprocessing(int,int,int,int,int);

// global variables 
double *coordinates;
int num_elements;
int *elements;
int nnodes,ndim;
double min_x,min_y, min_z,max_x,max_y,max_z;
double tolerance = 0.0001;

int main(){
int i,j,k,l,m;
char s[500], path[500], filename[500];

ndim = 3;
int TypeBC=1;
int SubTypeBC;
int AngleInc=25;
int PhiInc = 17;
FILE *submitjobs; 
FILE *LPK, *plotcombine, *combo;
combo = fopen("Combo", "w");
submitjobs = fopen("Submitjob", "w");
//eloutput = fopen("eloutput", "w");
LPK = fopen("LPK", "w");
plotcombine = fopen("CombinePlots", "w");
//fprintf(eloutput, "module load lsdyna\n");
//fprintf(eloutput, "rm YieldStrains.csv\n");
//fprintf(eloutput, "rm PrincipalStrains.csv\n");
//fprintf(eloutput, "rm PrincipalStresses.csv\n");
fprintf(plotcombine, "montage ");
fprintf(LPK, "module load tecplot\n");
for (i=0;i<1;i++){
    /* read micro */
    memset(s, 0, 50);
    sprintf(s,"mkdir Microstructure%d",i+1);
    system(s);    
    read_nodesandelements(i+1);
    find_minmax(i+1);
    for (j=0;j<TypeBC;j++){
         if(j==0){  //Normal Normal Loading
            sprintf(s,"mkdir Microstructure%d/TypeBC%d",i+1,j+1);
            system(s);
            printf("Running for Normal Normal loading\n");
            SubTypeBC=1;
            for (k=0;k<SubTypeBC;k++){
                 sprintf(s,"mkdir Microstructure%d/TypeBC%d/Subtype%d",i+1,j+1,k+1);
                 system(s);
                 if(k==0){
                   printf("Running for Subtype %d\n",k+1);
                   for(m=0;m<PhiInc;m++){
                         sprintf(s,"mkdir Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d",i+1,j+1,k+1,m+1);
                         system(s);
                         FILE *eloutput;
                         sprintf(filename, "eltout%d", m+1);
                         eloutput = fopen(filename, "w");
                      for(l=0;l<AngleInc;l++){
                         sprintf(s,"mkdir Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d",i+1,j+1,k+1,m+1,l+1);
                         system(s);
                        // write master
                           write_master(i+1,j+1,k+1,m+1,l+1);
                        // write nodes & elements
                           //write_nodeselements(i+1,j+1,k+1,m+1,l+1);
                        // write Bounday conditions
                           boundary_conditions(i+1,j+1,k+1,m+1,l+1);
                        // write load curves 
                           create_load_curves(i+1,j+1,k+1,m+1,l+1);
                        // write nodesets
                           write_nodesets(i+1,j+1,k+1,m+1,l+1);
                        // write submit files
                           write_submitfiles(i+1,j+1,k+1,m+1,l+1);
                        // Copy StressAveraging.c, YieldStrain.c and Plotting.c into each directory
                           write_postprocessing(i+1,j+1,k+1,m+1,l+1);
                        // create script to submit simulations
                           sprintf(s,"cd Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d\n",i+1,j+1,k+1,m+1,l+1);
                           fprintf(submitjobs, s);
                           fprintf(submitjobs, "rm d3* mes* *.pbs.o*\n");
                           fprintf(submitjobs, "qsub -A rhk12_collab Testcube.pbs\n");
                           fprintf(submitjobs,"cd ../../../../../\n");
                        // create script to convert bionout to elout, average stresses and create csv files and plot data
                           sprintf(s,"cd Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d\n",i+1,j+1,k+1,m+1,l+1);
                           fprintf(eloutput, s);
                           fprintf(eloutput, "/opt/aci/sw/lstc/mppdyna/R8.0.0/ls-dyna_mpp_s_r8_0_0_95359_x64_redhat54_ifort131_sse2_openmpi183.l2a binout*\n");
                           fprintf(eloutput, "gcc STRESS_Average.c\n");
                           fprintf(eloutput, "./a.out\n");
                           fprintf(eloutput, "gcc STRESS_NormalYieldCalcs.c -lm\n"); 
                           fprintf(eloutput, "./a.out\n");
                           fprintf(eloutput, "gcc STRESS_ShearYieldCalcs.c -lm\n");
                           fprintf(eloutput, "./a.out\n");
                           fprintf(eloutput, "gcc Plotting.c\n");
                           fprintf(eloutput, "./a.out\n");
                           fprintf(eloutput,"cd ../../../../../\n");
                        // LPK Script
                           //sprintf(path, "cp SingleMicro.lay Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d\n",i+1,j+1,k+1,m+1,l+1);
                           sprintf(s,"cd Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d\n",i+1,j+1,k+1,m+1,l+1);
                           //fprintf(LPK, path);
                           fprintf(LPK, s);
                           fprintf(LPK, "rm mes* eloutdet* binout* core* d3* Nodes*\n");
                           //fprintf(LPK, "tec360 -b -p LPKScript.mcr\n");
                           fprintf(LPK,"cd ../../../../../\n");
                        // create scipt to copy all stress and strains to one place
                           sprintf(s,"cd Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d\n",i+1,j+1,k+1,m+1,l+1);
                           fprintf(combo, s);
                           fprintf(combo, "gcc Combination.c -lm\n");
                           fprintf(combo, "./a.out\n");
                           fprintf(combo,"cd ../../../../../\n");
                        // create script to combine all png plots into a single PDF
                           sprintf(s,"Microstructure%d/TypeBC%d/Subtype%d//PhiInc%d/AngleInc%d/printme.png ",i+1,j+1,k+1,m+1,l+1);
                           fprintf(plotcombine, s);
                      }//loop on l
                   fclose(eloutput);
                   }//loop on m
                 }//if k=0
                 if(k==2 || k==1){
                    for(l=0;l<AngleInc;l++){
                     sprintf(s,"mkdir Microstructure%d/TypeBC%d/Subtype%d/AngleInc%d",i+1,j+1,k+1,l+1);
                     system(s);
                     // write master 
                        write_master(i+1,j+1,k+1,m+1,l+1);
                     // write nodes & elements
                        write_nodeselements(i+1,j+1,k+1,m+1,l+1);
                     // write Bounday conditions
                        boundary_conditions(i+1,j+1,k+1,m+1,l+1);
                     // write load curves
                        create_load_curves(i+1,j+1,k+1,m+1,l+1);
                     // write nodesets
                        write_nodesets(i+1,j+1,k+1,m+1,l+1);
                     // write submit files
                        write_submitfiles(i+1,j+1,k+1,m+1,l+1);
                     // Copy StressAveraging.c into each directory
                        write_postprocessing(i+1,j+1,k+1,m+1,l+1);
                     // create files to submit simulations
                        sprintf(s,"cd Microstructure%d/TypeBC%d/Subtype%d/AngleInc%d\n",i+1,j+1,k+1,l+1);
                        fprintf(submitjobs,s);
                        fprintf(submitjobs, "rm d3* *.lpk mes* *.pbs.o*\n");
                        fprintf(submitjobs, "qsub -A rhk12_collab Testcube.pb\n");
                        fprintf(submitjobs,"cd ../../../../\n");
                     // create script to convert binout to elout, average stresses, plot data, save to csv file
                        sprintf(s,"cd Microstructure%d/TypeBC%d/Subtype%d/AngleInc%d\n",i+1,j+1,k+1,l+1);
                        /*fprintf(eloutput, s);
                        fprintf(eloutput, "/opt/aci/sw/lstc/mppdyna/R8.0.0/ls-dyna_mpp_s_r8_0_0_95359_x64_redhat54_ifort131_sse2_openmpi183.l2a binout*\n");
                        fprintf(eloutput, "gcc STRESS_Average.c\n");
                        fprintf(eloutput, "./a.out\n");
                        fprintf(eloutput, "gcc STRESS_NormalYieldCalcs.c -lm\n");
                        fprintf(eloutput, "./a.out\n");
                        fprintf(eloutput, "gcc STRESS_ShearYieldCalcs.c -lm\n");
                        fprintf(eloutput, "./a.out\n");
                        fprintf(eloutput, "gcc Plotting.c\n");
                        fprintf(eloutput, "./a.out\n");
                        fprintf(eloutput,"cd ../../../../\n");*/
                     // LPK Script
                        //sprintf(path, "cp SingleMicro.lay Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d\n",i+1,j+1,k+1,m+1,l+1);
                        sprintf(s,"cd Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d\n",i+1,j+1,k+1,m+1,l+1);
                        //fprintf(LPK, path);
                        fprintf(LPK, s);
                        fprintf(LPK, "rm core* mes* d3* eloutdet* binout* Nodes*\n");
                        //fprintf(LPK, "tec360 -b -p LPKScript.mcr\n");
                        fprintf(LPK,"cd ../../../../../\n");
                     //create script to combine all stresses and strains
                        sprintf(s,"cd Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d\n",i+1,j+1,k+1,m+1,l+1);
                        fprintf(combo, s);
                        fprintf(combo, "gcc Combination.c -lm\n");
                        fprintf(combo, "./a.out\n");
                        fprintf(combo,"cd ../../../../../\n");
                     // create script to combine all png plots into a single PDF
                           sprintf(s,"Microstructure%d/TypeBC%d/Subtype%d/AngleInc%d/printme.png ",i+1,j+1,k+1,l+1);
                           fprintf(plotcombine, s);
                 } //loop on l  
                 }//if k=1 or k=2
            }//loop on k
         }//if j=0
         if(j==1){  //Normal Shear Loading
            sprintf(s,"mkdir Microstructure%d/TypeBC%d",i+1,j+1);
            system(s);
            SubTypeBC=9;
            printf("Running for Normal-Shear loading\n");
             for (k=0;k<SubTypeBC;k++){
                 sprintf(s,"mkdir Microstructure%d/TypeBC%d/Subtype%d",i+1,j+1,k+1);
                 system(s);
                 printf("Running for SubType %d\n", k+1);
                 AngleInc = 9;
                 for(l=0;l<AngleInc;l++){
                     sprintf(s,"mkdir Microstructure%d/TypeBC%d/Subtype%d/AngleInc%d",i+1,j+1,k+1,l+1);
                     system(s);
                     // write master 
                        write_master(i+1,j+1,k+1,m+1,l+1);
                     // write nodes & elements
                        //write_nodeselements(i+1,j+1,k+1,m+1,l+1);
                     // write Bounday conditions
                        boundary_conditions(i+1,j+1,k+1,m+1,l+1);
                     // write load curves
                        create_load_curves(i+1,j+1,k+1,m+1,l+1);
                     // write nodesets
                        write_nodesets(i+1,j+1,k+1,m+1,l+1);
                     // write submit files
                        write_submitfiles(i+1,j+1,k+1,m+1,l+1);
                     // Copy StressAveraging.c into each directory
                        write_postprocessing(i+1,j+1,k+1,m+1,l+1);
                     // create files to submit simulations
                        sprintf(s,"cd Microstructure%d/TypeBC%d/Subtype%d/AngleInc%d\n",i+1,j+1,k+1,l+1);
                        fprintf(submitjobs,s);
                        fprintf(submitjobs, "rm d3dump* binout* eloutdet* mes* *.pbs.o*\n");
                        fprintf(submitjobs, "qsub Testcube.pbs\n");
                        fprintf(submitjobs,"cd ../../../../\n");
                     // create script to convert binout to elout, average stresses, plot data, save to csv file
                        /*sprintf(s,"cd Microstructure%d/TypeBC%d/Subtype%d/AngleInc%d\n",i+1,j+1,k+1,l+1);
                        fprintf(eloutput, s);
                        fprintf(eloutput, "/usr/cta/ls-dyna/971_R8.0/l2a binout*\n");
                        fprintf(eloutput, "gcc STRESS_Average.c\n");
                        fprintf(eloutput, "./a.out\n");
                        fprintf(eloutput, "gcc STRESS_NormalYieldCalcs.c -lm\n");
                        fprintf(eloutput, "./a.out\n");
                        fprintf(eloutput, "gcc STRESS_ShearYieldCalcs.c -lm\n");
                        fprintf(eloutput, "./a.out\n");
                        fprintf(eloutput, "gcc Plotting.c\n");
                        fprintf(eloutput, "./a.out\n");
                        fprintf(eloutput,"cd ../../../../\n");*/
                     // LPK Script
                        //sprintf(path, "cp SingleMicro.lay Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d\n",i+1,j+1,k+1,m+1,l+1);
                        //sprintf(s,"cd Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d\n",i+1,j+1,k+1,m+1,l+1);
                        //fprintf(LPK, path);
                        //fprintf(LPK, s);
                        //fprintf(LPK, "rm mes* core* d3* Nodes* eloutdet* binout*\n");
                        //fprintf(LPK, "tec360 -b -p LPKScript.mcr\n");
                        //fprintf(LPK,"cd ../../../../../\n");
                     // create script to combine all stresses and strains into one file/place
                        sprintf(s,"cd Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d\n",i+1,j+1,k+1,m+1,l+1);
                        fprintf(combo, s);
                        fprintf(combo, "gcc Combination.c -lm\n");
                        fprintf(combo, "./a.out\n");
                        fprintf(combo,"cd ../../../../../\n");
                     // create script to combine all png plots into a single PDF
                        sprintf(s,"Microstructure%d/TypeBC%d/Subtype%d/AngleInc%d/printme.png ",i+1,j+1,k+1,l+1);
                        fprintf(plotcombine, s);
                 } //loop on l
            }//loop on k
         } //if j=1
    }//end loop on j
}//end for loop on i

fprintf(plotcombine, "-tile 3x3 -geometry 1000 CombinedPlots.pdf\n");
//fprintf(plotcombine, "gcc YieldSurfacePlotting.c\n");
//fprintf(plotcombine, "./a.out\n");

fclose(plotcombine);
//fclose(eloutput);
fclose(submitjobs);
fclose(LPK);
}//end main
int find_minmax(int microN){
//microN = 3;
/*Find maximum and minimum coordinates of nodes*/
max_x =-1e20;
max_y =-1e20;
max_z =-1e20;
int tt;
for(tt=0;tt<nnodes;tt++){
   if (coordinates[ndim*tt+0]>max_x){
       max_x = coordinates[ndim*tt+0];
   }
   if (coordinates[ndim*tt+1]>max_y){
       max_y = coordinates[ndim*tt+1];
   }
   if (coordinates[ndim*tt+2]>max_z){
       max_z = coordinates[ndim*tt+2];
   }
}
min_x=1E20;
min_y=1E20;
min_z=1E20;

for(tt=0;tt<nnodes;tt++){
   if (coordinates[ndim*tt+0]<min_x){
       min_x = coordinates[ndim*tt+0];
   }
   if (coordinates[ndim*tt+1]<min_y){
       min_y = coordinates[ndim*tt+1];
   }
   if (coordinates[ndim*tt+2]<min_z){
       min_z = coordinates[ndim*tt+2];
   }
}

printf("Min coordinates are x=%f y=%f z=%f\n", min_x, min_y, min_z);
printf("Max coordinates are x=%f y=%f z=%f\n", max_x, max_y, max_z);

} // end of find minmax function

int write_master(int microN, int j, int k, int m, int l){
FILE *fp1, *master, *title;
char s[100]; 
char t[100], T[600];
char a;
fp1 = fopen("Master.k", "r");
//microN = 3;

if(j==1 && k==1)
  {sprintf(s,"Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d/Master.k",microN,j,k,m,l);}
else{sprintf(s,"Microstructure%d/TypeBC%d/Subtype%d/AngleInc%d/Master.k",microN,j,k,l);}
master = fopen(s, "w");

if(j==1 && k==1)
  {sprintf(t,"Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d/Title.txt",microN,j,k,m,l);}
else{sprintf(t,"Microstructure%d/TypeBC%d/Subtype%d/AngleInc%d/Title.txt",microN,j,k,l);}
title = fopen(t, "w");

do{
a = fgetc(fp1);
if(a==EOF){break;}
fputc(a,master);
} while (a !=EOF);


double theta[25]={0, 0.2618, 0.5236, 0.7854, 1.0472, 1.3090, 1.5708, 1.8326, 2.0944, 2.3562, 2.618, 2.879, 3.142, 3.403, 3.665, 3.9270, 4.189, 4.451, 4.712, 4.974, 5.236, 5.498, 5.759, 6.021, 6.283};
double phi[17] = {0,0.2094,0.4189,0.6283,0.8378,1.0472,1.2566,1.4661,1.5708, 1.6755,1.8850,2.0944,2.3038,2.5133,2.7227,2.9322,3.14159};

double cosphi = fabs(cos(phi[m-1]));
if (fabs(cosphi)<0.001){cosphi = round(cosphi);}
double sinphi = fabs(sin(phi[m-1]));
if (fabs(sinphi)<0.001){sinphi = round(sinphi);}
double costheta = fabs(cos(theta[l-1]));
if (fabs(costheta)<0.001){costheta = round(costheta);}
double sintheta = fabs(sin(theta[l-1]));
if (fabs(sintheta)<0.001){sintheta = round(sintheta);}


if(j==1){ 
  //UNIAXIAL CASES
  //X
  if(cosphi==0  && sintheta==0){
  //if((m==5 || m==13) && (l==1 || l==9)){
   sprintf(T, "Uniaxial Loading in X Direction: Theta=%d & Phi=%d", l,m);
   fprintf(title, "%s", T);
   //printf("Uniaxial x- %d\n", (m-1)*25 + l);
   //printf("Phi %d AI %d\n", m, l);
  }

  //Y
  else if (costheta==0 && sintheta!=0 && cosphi==0){
      sprintf(T, "Uniaxial Loading in Y Direction: Theta=%d & Phi=%d", l,m);
      fprintf(title, "%s", T);
      //printf("Uniaxial y- %d\n", (m-1)*25 + l);
      //printf("Phi %d AI %d\n", m, l); 
  }
  
  //Z
  else if ((sinphi==0 || costheta==0) && (sinphi==0 || sintheta==0) && cosphi!=0){
      sprintf(T, "Uniaxial Loading in Z Direction: Theta=%d & Phi=%d", l,m);
      fprintf(title, "%s", T);
      //printf("Uniaxial Z- %d\n", (m-1)*25 + l);
      //printf("Phi %d AI %d\n", m, l);
  }

  //BIAXIAL CASES
  //XY
  else if (cosphi==0 && sinphi!=0){
      sprintf(T, "Biaxial in X & Y Directions: Theta=%d & Phi=%d", l,m);
      fprintf(title, "%s", T);
      //printf("Biaxial xy- %d\n", (m-1)*25 + l);
  }//End XY
  
  //XZ
  else if (sintheta==0 && sinphi!=0 && costheta!=0 && cosphi!=0){
      sprintf(T, "Biaxial in X & Z Directions Theta=%d & Phi=%d", l,m);
      fprintf(title, "%s", T);
      //printf("Biaxial xz- %d\n", (m-1)*25 + l);
      //printf("Phi %d AI %d\n", m, l);
  }

  //YZ
  else if (sinphi!=0 && sintheta!=0 && cosphi!=0 && costheta==0){
      sprintf(T, "Biaxial in Y & Z Directions: Theta=%d & Phi=%d", l,m);
      fprintf(title, "%s", T);
      //printf("Biaxial yz- %d\n", (m-1)*25 + l);
      //printf("Phi %d AI %d\n", m, l);
  }//end YZ

  //Triaxial
  else{
      sprintf(T, "Triaxial Loading: Theta=%d & Phi=%d", l,m);
      fprintf(title, "%s", T);
      //printf("Triaxial- %d\n", (m-1)*25 + l);
  }//end triaxial
}//end j==1

if(j==2){
  if(k==1){
    if(l==5){
       sprintf(T, "Simple Shear XY");
       fprintf(title, "%s", T);
    }
    else{
        sprintf(T, "Normal Loading on X faces + Shear XY-AngleInc%d",l);
        fprintf(title, "%s", T);
    }
  }

  if(k==2){
    if(l==5){
       sprintf(T, "Simple Shear YZ");
       fprintf(title, "%s", T);
    }
    else{
        sprintf(T, "Normal Loading on X faces + Shear YZ-AngleInc%d",l);
        fprintf(title, "%s", T);
    }
  }

  if(k==3){
    if(l==5){
       sprintf(T, "Simple Shear XZ");
       fprintf(title, "%s", T);
    }
    else{
        sprintf(T, "Normal Loading on X faces + Shear XZ-AngleInc%d",l);
        fprintf(title, "%s", T);
    }
  }
 
  if(k==4){
    if(l==5){
       sprintf(T, "Simple Shear XY");
       fprintf(title, "%s", T);
    }
    else{
        sprintf(T, "Normal Loading on Y faces + Shear XY-AngleInc%d",l);
        fprintf(title, "%s", T);
    }
  }

  if(k==5){ 
    if(l==5){
       sprintf(T, "Simple Shear YZ");
       fprintf(title, "%s", T);
    }
    else{
        sprintf(T, "Normal Loading on Y faces + Shear YZ-AngleInc%d",l);
        fprintf(title, "%s", T);
    }
  }

  if(k==6){
    if(l==5){
       sprintf(T, "Simple Shear XZ");
       fprintf(title, "%s", T);
    }
    else{
        sprintf(T, "Normal Loading on Y faces + Shear XZ-AngleInc%d",l);
        fprintf(title, "%s", T);
    }
  }

    if(k==7){
    if(l==5){
       sprintf(T, "Simple Shear XY");
       fprintf(title, "%s", T);
    }
    else{
        sprintf(T, "Normal Loading on Z faces + Shear XY-AngleInc%d",l);
        fprintf(title, "%s", T);
    }
  }

  if(k==8){
    if(l==5){
       sprintf(T, "Simple Shear YZ");
       fprintf(title, "%s", T);
    }
    else{
        sprintf(T, "Normal Loading on Z faces + Shear YZ-AngleInc%d",l);
        fprintf(title, "%s", T);
    }
  }

  if(k==9){
    if(l==5){
       sprintf(T, "Simple Shear XZ");
       fprintf(title, "%s", T);
    }
    else{
        sprintf(T, "Normal Loading on Z faces + Shear XZ-AngleInc%d",l);
        fprintf(title, "%s", T);
    }
  }
}
fclose(fp1);
fclose(master);
fclose(title);
}//end of write master function

int write_postprocessing(int microN, int j, int k, int m, int l){
FILE *fp1, *fp2, *fp3, *fp4, *fp5, *averaging, *script, *plotting, *yield, *normalyield, *combining;
char s[500], h[500];
char r[500], q[500], t[500];
char a, b, c, d, e;
fp1=fopen("STRESS_Average.c", "r");
fp2=fopen("Plotting.c", "r");
fp3=fopen("STRESS_ShearYieldCalcs.c", "r");
fp4=fopen("STRESS_NormalYieldCalcs.c", "r");
fp5=fopen("Combination.c", "r");


if(j==1 && k==1)
  {sprintf(s,"Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d/LPKScript.mcr",microN,j,k,m,l);}
else{sprintf(s,"Microstructure%d/TypeBC%d/Subtype%d/AngleInc%d/LPKscript.mcr",microN,j,k,l);}
script = fopen(s, "w");
if(j!=1 && k!=1){
printf("Micro = %d Type BC = %d Subtype = %d AI = %d\n",microN, j, k, l); } 

if(j==1 && k==1){
fprintf(script, "#!MC 1410\n");
fprintf(script, "$!VarSet |MFBD| = '/storage/home/anr5202/scratch/NewYieldSurface/%dMICRO/Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d'\n", microN, microN, j, k, m, l);
fprintf(script, "$!PICK SETMOUSEMODE\n");
fprintf(script, "MOUSEMODE = SELECT\n");
fprintf(script, "$!PAGE NAME = 'Untitled'\n");
fprintf(script, "$!PAGECONTROL CREATE\n");
fprintf(script, "$!PICK SETMOUSEMODE\n");
fprintf(script, "  MOUSEMODE = SELECT\n");
fprintf(script, "$!OPENLAYOUT '/storage/home/anr5202/scratch/NewYieldSurface/%dMICRO/Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d/SingleMicro.lay'\n", microN, microN, j, k, m, l);
fprintf(script, "$!SAVELAYOUT '/storage/home/anr5202/scratch/NewYieldSurface/%dMICRO/Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d/Microstructure.lpk'\n", microN, microN, j, k, m, l);
fprintf(script, "  INCLUDEDATA = YES\n INCLUDEPREVIEW = NO\n $!RemoveVar |MFBD|\n");}
else{
fprintf(script, "#!MC 1410\n");
fprintf(script, "$!VarSet |MFBD| = '/storage/home/anr5202/scratch/NewYieldSurface/%dMICRO/Microstructure%d/TypeBC%d/Subtype%d/AngleInc%d'\n",microN,microN,j,k,l);
fprintf(script, "$!PICK SETMOUSEMODE\n");
fprintf(script, "MOUSEMODE = SELECT\n");
fprintf(script, "$!PAGE NAME = 'Untitled'\n");
fprintf(script, "$!PAGECONTROL CREATE\n");
fprintf(script, "$!PICK SETMOUSEMODE\n");
fprintf(script, "  MOUSEMODE = SELECT\n");
fprintf(script, "$!OPENLAYOUT '/storage/home/anr5202/scratch/NewYieldSurface/%dMICRO/Microstructure%d/TypeBC%d/Subtype%d/AngleInc%d/SingleMicro.lay'\n", microN,microN,j,k,l);
fprintf(script, "$!SAVELAYOUT '/storage/home/anr5202/scratch/NewYieldSurface/%dMICRO/Microstructure%d/TypeBC%d/Subtype%d/AngleInc%d/Microstructure.lpk'\n", microN,microN,j,k,l);
fprintf(script, "  INCLUDEDATA = YES\n INCLUDEPREVIEW = NO\n $!RemoveVar |MFBD|\n");}
fclose(script);

if(j==1 && k==1)
  {sprintf(s,"Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d/STRESS_Average.c",microN,j,k,m,l);}
else{sprintf(s,"Microstructure%d/TypeBC%d/Subtype%d/AngleInc%d/STRESS_Average.c",microN,j,k,l);}
averaging = fopen(s, "w");

if(j==1 && k==1)
  {sprintf(q,"Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d/STRESS_ShearYieldCalcs.c",microN,j,k,m,l);}
else{sprintf(q,"Microstructure%d/TypeBC%d/Subtype%d/AngleInc%d/STRESS_ShearYieldCalcs.c",microN,j,k,l);}
yield = fopen(q, "w");

if(j==1 && k==1)
  {sprintf(h,"Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d/STRESS_NormalYieldCalcs.c",microN,j,k,m,l);}
else{sprintf(h,"Microstructure%d/TypeBC%d/Subtype%d/AngleInc%d/STRESS_NormalYieldCalcs.c",microN,j,k,l);}
normalyield = fopen(h, "w");


if(j==1 && k==1)
  {sprintf(r,"Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d/Plotting.c",microN,j,k,m,l);}
else{sprintf(r,"Microstructure%d/TypeBC%d/Subtype%d/AngleInc%d/Plotting.c",microN,j,k,l);}
plotting = fopen(r, "w");

if(j==1 && k==1)
   {sprintf(t,"Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d/Combination.c",microN,j,k,m,l);}
else{sprintf(t,"Microstructure%d/TypeBC%d/Subtype%d/AngleInc%d/Combination.c",microN,j,k,l);}
combining = fopen(t, "w");


do{
a = fgetc(fp1);
if(a==EOF){break;}
fputc(a,averaging);
} while (a !=EOF);
fclose(fp1);
fclose(averaging);

do{
b = fgetc(fp2);
if(b==EOF){break;}
fputc(b,plotting);
}while (b !=EOF);
fclose(fp2);
fclose(plotting);


do{
d = fgetc(fp4);
if(d==EOF){break;}
fputc(d,normalyield);
}while (d !=EOF);
fclose(fp4);
fclose(normalyield);

do{
c = fgetc(fp3);
if(c==EOF){break;}
fputc(c, yield);
}while (c !=EOF);
fclose(fp3);
fclose(yield);

do{
e = fgetc(fp5);
if(e==EOF){break;}
fputc(e, combining);
}while (e !=EOF);
fclose(fp5);
fclose(combining);

}//end of write post-processing files

//WRITE FILES TO SUBMIT SIMULATIONS
int write_submitfiles(int microN, int j, int k, int m, int l){
FILE *fp1, *fp2, *pbs, *pfile;
char q[200], h[200];
char a,b;
fp1 = fopen("Testcube.pbs", "r");
fp2 = fopen("pfile", "r");

if(j==1 && k==1)
  {sprintf(q,"Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d/pfile",microN,j,k,m,l);}
else if(j==2){sprintf(q,"Microstructure%d/TypeBC%d/Subtype%d/AngleInc%d/pfile",microN,j,k,l);}
pfile = fopen(q, "w");

if(j==1 && k==1)
  {sprintf(h,"Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d/Testcube.pbs",microN,j,k,m,l);}
else{sprintf(h,"Microstructure%d/TypeBC%d/Subtype%d/AngleInc%d/Testcube.pbs",microN,j,k,l);}
pbs = fopen(h, "w");

do{
a = fgetc(fp2);
if(a==EOF){break;}
fputc(a,pfile);
} while (a !=EOF);

do{
b = fgetc(fp1);
if(b==EOF){break;}
fputc(b,pbs);
} while (b !=EOF);

fclose(fp1);
fclose(fp2);
fclose(pfile);
fclose(pbs);

}//end of write_submitfiles function


int write_nodesets(int microN,int jj,int kk, int mm, int ll){
/*Select faces for boundary conditions*/

/*X faces*/
FILE *nodeset;
int sid = 1;
int count = 0;
char s[500];
int i; 


if(jj==1 && kk==1)
  {sprintf(s,"Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d/Nodesets.k",microN,jj,kk,mm,ll);}
else{sprintf(s,"Microstructure%d/TypeBC%d/Subtype%d/AngleInc%d/Nodesets.k",microN,jj,kk,ll);}
nodeset = fopen(s, "w");


fprintf(nodeset, "*SET_NODE_LIST_TITLE\n");
fprintf(nodeset,"X MAX Nodes\n");
fprintf(nodeset, "$#     sid       da1       da2       da3       da4    solver\n");
fprintf(nodeset, "         %d     0.000     0.000     0.000     0.000      MECH\n", sid);
fprintf(nodeset, "$#    nid1      nid2      nid3      nid4      nid5      nid6      nid7      nid8\n");
for(i=1;i<nnodes+1;i++){  
     if(coordinates[ndim*(i-1)+0]>max_x-1.5*tolerance){
     count = count + 1;
        if(count<8){
          fprintf(nodeset,"%10.0d",i);
        } 
        else{
          fprintf(nodeset,"%10.0d\n",i);
          count = 0;
        }
    }
}//end of for loop on i for xnodes
sid = 2;
count = 0;
fprintf(nodeset, "\n*SET_NODE_LIST_TITLE\n");
fprintf(nodeset,"X MIN Nodes\n");
fprintf(nodeset, "$#     sid       da1       da2       da3       da4    solver\n");
fprintf(nodeset, "         %d     0.000     0.000     0.000     0.000      MECH\n", sid);
fprintf(nodeset, "$#    nid1      nid2      nid3      nid4      nid5      nid6      nid7      nid8\n");

for(i=1;i<nnodes+1;i++){  
     if(coordinates[ndim*(i-1)+0]-min_x<2.5*tolerance){
        count = count + 1;
        if(count<8){
          fprintf(nodeset,"%10.0d",i);
        } 
        else{
          fprintf(nodeset,"%10.0d\n",i);
          count = 0;
        }
   }
}//end of for loop on i for xnodes

/*Y faces*/
//FILE *nodesy;
sid = 3;
count = 0;
fprintf(nodeset, "\n*SET_NODE_LIST_TITLE\n");
fprintf(nodeset,"Y MAX Nodes\n");
fprintf(nodeset, "$#     sid       da1       da2       da3       da4    solver\n");
fprintf(nodeset, "         %d     0.000     0.000     0.000     0.000      MECH\n", sid);
fprintf(nodeset, "$#    nid1      nid2      nid3      nid4      nid5      nid6      nid7      nid8\n");
for(i=1;i<nnodes+1;i++){  
     if(coordinates[ndim*(i-1)+1]>max_y-2*tolerance){
     count = count + 1;
     if(count<8){
       fprintf(nodeset,"%10.0d",i);
     } 
     else{
         fprintf(nodeset,"%10.0d\n",i);
          count = 0;
     }
     }
}//end of for loop on i for ynodes

sid = 4;
count = 0;
fprintf(nodeset, "\n*SET_NODE_LIST_TITLE\n");
fprintf(nodeset,"Y MIN Nodes\n");
fprintf(nodeset, "$#     sid       da1       da2       da3       da4    solver\n");
fprintf(nodeset, "         %d     0.000     0.000     0.000     0.000      MECH\n", sid);
fprintf(nodeset, "$#    nid1      nid2      nid3      nid4      nid5      nid6      nid7      nid8\n");
for(i=1;i<nnodes+1;i++){  
     if(coordinates[ndim*(i-1)+1]-min_y<tolerance){
     count = count + 1;
     if(count<8){
       fprintf(nodeset,"%10.0d",i);
     } 
     else{
         fprintf(nodeset,"%10.0d\n",i);
          count = 0;
     }
   }
}//end of for loop on i for ynodes

/*Z faces*/
//FILE *nodesz;
sid = 5;
count = 0;
fprintf(nodeset, "\n*SET_NODE_LIST_TITLE\n");
fprintf(nodeset,"Z MAX Nodes\n");
fprintf(nodeset, "$#     sid       da1       da2       da3       da4    solver\n");
fprintf(nodeset, "         %d     0.000     0.000     0.000     0.000      MECH\n", sid);
fprintf(nodeset, "$#    nid1      nid2      nid3      nid4      nid5      nid6      nid7      nid8\n");
for(i=1;i<nnodes+1;i++){  
     if(coordinates[ndim*(i-1)+2]>max_z-2*tolerance){
     count = count + 1;
     if(count<8){
       fprintf(nodeset,"%10.0d",i);
     } 
     else{
         fprintf(nodeset,"%10.0d\n",i);
          count = 0;
     }
     }
}//end of for loop on i for znodes

sid = 6;
count = 0;
fprintf(nodeset, "\n*SET_NODE_LIST_TITLE\n");
fprintf(nodeset,"Z MIN Nodes\n");
fprintf(nodeset, "$#     sid       da1       da2       da3       da4    solver\n");
fprintf(nodeset, "         %d     0.000     0.000     0.000     0.000      MECH\n", sid);
fprintf(nodeset, "$#    nid1      nid2      nid3      nid4      nid5      nid6      nid7      nid8\n");
for(i=1;i<nnodes+1;i++){  
     if(coordinates[ndim*(i-1)+2]-min_z<3*tolerance){
     count = count + 1;
     if(count<8){
       fprintf(nodeset,"%10.0d",i);
     } 
     else{
         fprintf(nodeset,"%10.0d\n",i);
          count = 0;
     }
     }
}//end of for loop on i for znodes

fclose(nodeset);

} /* end of write_nodesets function */

/*CREATE LOAD CURVES*/
int create_load_curves(int microN,int j, int k, int m, int l){
FILE *fp3;
char s[300];
double Xx, Yy, Zz;
double x[256], y[256],z[256],a,b, Z;
int c;
double edge_length, max_displacement;

double theta[25]={0, 0.2618, 0.5236, 0.7854, 1.0472, 1.3090, 1.5708, 1.8326, 2.0944, 2.3562, 2.618, 2.879, 3.142, 3.403, 3.665, 3.9270, 4.189, 4.451, 4.712, 4.974, 5.236, 5.498, 5.759, 6.021, 6.283};
double phi[17] = {0,0.2094,0.4189,0.6283,0.8378,1.0472,1.2566,1.4661,1.5708, 1.6755,1.8850,2.0944,2.3038,2.5133,2.7227,2.9322,3.14159};


double cosphi = 0.5E-4*cos(phi[m-1]);
if (fabs(cosphi)<1E-6){cosphi = round(cosphi);}
double sinphi = 0.5E-4*sin(phi[m-1]);
if (fabs(sinphi)<1E-6){sinphi = round(sinphi);}
double costheta = cos(theta[l-1]);
if (fabs(costheta)<0.001){costheta = round(costheta);}
double sintheta = sin(theta[l-1]);
if (fabs(sintheta)<0.001){sintheta = round(sintheta);}

Xx = sinphi*costheta;
Yy = sinphi*sintheta;
Zz = cosphi;

//printf("x = %e  y = %e  z = %e\n", Xx, Yy, Zz);

if(j==2){
  double Theta[9]={0.314159, 0.62831, 0.942477, 1.2566, 1.5708, 1.88495, 2.1991, 2.51327, 2.82743};
  double X[9],Y[9];
  for(c=0;c<9;c++){
     X[c]=1.5E-4*cos(Theta[c]);
     Y[c]=1.5E-4*sin(Theta[c]);
  }
  if(l==1){a=4*X[0]; b=4*Y[0];}
  if(l==2){a=2*X[1]; b=2*Y[1];}
  if(l==3){a=2*X[2]; b=2*Y[2];}
  if(l==4){a=2*X[3]; b=2*Y[3];}
  if(l==5){a=2*X[4]; b=2*Y[4];}
  if(l==6){a=2*X[5]; b=2*Y[5];}
  if(l==7){a=2*X[6]; b=2*Y[6];}
  if(l==8){a=2*X[7]; b=2*Y[7];}
  if(l==9){a=4*X[8]; b=4*Y[8];}
}

if(j==1){
  //UNIAXIAL CASES

  //X
  if(cosphi==0 && sinphi!=0 && sintheta==0 && costheta!=0){
  //if((m==5 || m==13) && (l==1 || l==9)){
    char title[20];
    sprintf(s,"Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d/LoadCurves.k",microN,j,k,m,l);
    fp3 = fopen(s, "w");
    fprintf(fp3, "*DEFINE_CURVE\n");
    fprintf(fp3, "$            absissa            ordinate\n");
    fprintf(fp3, "$#    lcid      sidr       sfa       sfo      offa      offo    dattyp\n");
    fprintf(fp3, "         1         0  1.000000  1.000000     0.000     0.000         0\n");
    fprintf(fp3, "$#                a1                  o1\n");
    fprintf(fp3, "               0.000               0.000\n");
    fprintf(fp3, "             0.45E-3      %14.3e\n",Xx); 
  }

  //Y
  else if (costheta==0 && sintheta!=0 && cosphi==0 && sinphi!=0){
  //else if((m==5 || m==13) && (l==5 || l==13)){
    char title[20];
    sprintf(s,"Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d/LoadCurves.k",microN,j,k,m,l);
    fp3 = fopen(s, "w");
    fprintf(fp3, "*DEFINE_CURVE\n");
    fprintf(fp3, "$            absissa            ordinate\n");
    fprintf(fp3, "$#    lcid      sidr       sfa       sfo      offa      offo    dattyp\n");
    fprintf(fp3, "         1         0  1.000000  1.000000     0.000     0.000         0\n");
    fprintf(fp3, "$#                a1                  o1\n");
    fprintf(fp3, "               0.000               0.000\n");
    fprintf(fp3, "             0.45E-3      %14.3e\n",Yy); 
  }
  
  //Z
  else if ((sinphi==0 || costheta==0) && (sinphi==0 || sintheta==0)){
  //else if(m==1 || m==9){
    char title[20];
    sprintf(s,"Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d/LoadCurves.k",microN,j,k,m,l);
    fp3 = fopen(s, "w");
    fprintf(fp3, "*DEFINE_CURVE\n");
    fprintf(fp3, "$            absissa            ordinate\n");
    fprintf(fp3, "$#    lcid      sidr       sfa       sfo      offa      offo    dattyp\n");
    fprintf(fp3, "         1         0  1.000000  1.000000     0.000     0.000         0\n");
    fprintf(fp3, "$#                a1                  o1\n");
    fprintf(fp3, "               0.000               0.000\n");
    fprintf(fp3, "             0.45E-3      %14.3e\n",Zz);
    //printf("z load = %e\n", (m-1)*16 + l-1);  
}

  //BIAXIAL CASES
  
  //XY
  else if (cosphi==0 && sinphi!=0 && costheta!=0 && sintheta!=0){
  //else if((m==5 || m==13) && l!=1 && l!=5 && l!=9 && l!=13){
     char title[20];
     sprintf(s,"Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d/LoadCurves.k",microN,j,k,m,l);
     fp3 = fopen(s, "w");
     fprintf(fp3, "*DEFINE_CURVE\n");
     fprintf(fp3, "$            absissa            ordinate\n");
     fprintf(fp3, "$#    lcid      sidr       sfa       sfo      offa      offo    dattyp\n");
     fprintf(fp3, "         1         0         1         1         0         0        0\n");
     fprintf(fp3, "$#                a1                  o1\n");
     fprintf(fp3, "               0.000               0.000\n");
     fprintf(fp3, "             0.45E-3      %14.3e\n",Xx);
     fprintf(fp3, "*DEFINE_CURVE\n");
     fprintf(fp3, "$            absissa            ordinate\n");
     fprintf(fp3, "$#    lcid      sidr       sfa       sfo      offa      offo    dattyp\n");
     fprintf(fp3, "         2         0         1         1     0.000     0.000         0\n");
     fprintf(fp3, "$#                a1                  o1\n");
     fprintf(fp3, "               0.000               0.000\n");
     fprintf(fp3, "             0.45E-3      %14.3e\n",Yy); 
  }//End XY
  
  //XZ
  else if (sintheta==0 && sinphi!=0 && costheta!=0 && cosphi!=0){
  //else if((l==1 || l==9) && m!=5 && m!=13 && m!=1 && m!=9){
     char title[20];
     sprintf(s,"Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d/LoadCurves.k",microN,j,k,m,l);
     fp3 = fopen(s, "w");
     fprintf(fp3, "*DEFINE_CURVE\n");
     fprintf(fp3, "$            absissa            ordinate\n");
     fprintf(fp3, "$#    lcid      sidr       sfa       sfo      offa      offo    dattyp\n");
     fprintf(fp3, "         1         0         1         1         0         0        0\n");
     fprintf(fp3, "$#                a1                  o1\n");
     fprintf(fp3, "               0.000               0.000\n");
     fprintf(fp3, "             0.45E-3      %14.3e\n",Xx);
     fprintf(fp3, "*DEFINE_CURVE\n");
     fprintf(fp3, "$            absissa            ordinate\n");
     fprintf(fp3, "$#    lcid      sidr       sfa       sfo      offa      offo    dattyp\n");
     fprintf(fp3, "         2         0         1         1     0.000     0.000         0\n");
     fprintf(fp3, "$#                a1                  o1\n");
     fprintf(fp3, "               0.000               0.000\n");
     fprintf(fp3, "             0.45E-3      %14.3e\n",Zz); 
  }//End XZ
  
  //YZ
  else if (sinphi!=0 && sintheta!=0 && cosphi!=0 && costheta==0){
  //else if((l==5 || l==13) && m!=5 && m!=13 && m!=1 && m!=9){
     char title[20];
     sprintf(s,"Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d/LoadCurves.k",microN,j,k,m,l);
     fp3 = fopen(s, "w");
     fprintf(fp3, "*DEFINE_CURVE\n");
     fprintf(fp3, "$            absissa            ordinate\n");
     fprintf(fp3, "$#    lcid      sidr       sfa       sfo      offa      offo    dattyp\n");
     fprintf(fp3, "         1         0         1         1         0         0        0\n");
     fprintf(fp3, "$#                a1                  o1\n");
     fprintf(fp3, "               0.000               0.000\n");
     fprintf(fp3, "             0.45E-3      %14.3e\n",Yy);
     fprintf(fp3, "*DEFINE_CURVE\n");
     fprintf(fp3, "$            absissa            ordinate\n");
     fprintf(fp3, "$#    lcid      sidr       sfa       sfo      offa      offo    dattyp\n");
     fprintf(fp3, "         2         0         1         1     0.000     0.000         0\n");
     fprintf(fp3, "$#                a1                  o1\n");
     fprintf(fp3, "               0.000               0.000\n");
     fprintf(fp3, "             0.45E-3      %14.3e\n",Zz); 
  }//end YZ

  //Triaxial
  else if (sinphi!=0 && cosphi!=0 && costheta!=0 && sintheta!=0){
  //else (m!=1 && m!=5 && m!=9 && m!=13 && l!=1 && l!=5 && l!=9 && l!=13){ 
     sprintf(s,"Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d/LoadCurves.k",microN,j,k,m,l);
     fp3 = fopen(s, "w");
     fprintf(fp3, "*DEFINE_CURVE\n");
     fprintf(fp3, "$            absissa            ordinate\n");
     fprintf(fp3, "$#    lcid      sidr       sfa       sfo      offa      offo    dattyp\n");
     fprintf(fp3, "         1         0         1         1         0         0        0\n");
     fprintf(fp3, "$#                a1                  o1\n");
     fprintf(fp3, "               0.000               0.000\n");
     fprintf(fp3, "             0.45E-3      %14.3e\n",Xx);
     fprintf(fp3, "*DEFINE_CURVE\n");
     fprintf(fp3, "$            absissa            ordinate\n");
     fprintf(fp3, "$#    lcid      sidr       sfa       sfo      offa      offo    dattyp\n");
     fprintf(fp3, "         2         0         1         1     0.000     0.000         0\n");
     fprintf(fp3, "$#                a1                  o1\n");
     fprintf(fp3, "               0.000               0.000\n");
     fprintf(fp3, "             0.45E-3      %14.3e\n",Yy);
     fprintf(fp3, "*DEFINE_CURVE\n");
     fprintf(fp3, "$            absissa            ordinate\n");
     fprintf(fp3, "$#    lcid      sidr       sfa       sfo      offa      offo    dattyp\n");
     fprintf(fp3, "         3         0         1         1     0.000     0.000         0\n");
     fprintf(fp3, "$#                a1                  o1\n");
     fprintf(fp3, "               0.000               0.000\n");
     fprintf(fp3, "             0.45E-3      %14.3e\n",Zz);  
  }//end triaxial
}//end j==1

//SHEAR CASES
if(j==2){
   //NORMAL SHEAR ON DIFFERENT FACES
   if(k==2 || k==6 || k==7){
     if(l==5){   //SIMPLE SHEAR
       char title[20];
       sprintf(s,"Microstructure%d/TypeBC%d/Subtype%d/AngleInc%d/LoadCurves.k",microN,j,k,l);
       fp3 = fopen(s, "w");
       fprintf(fp3, "*DEFINE_CURVE\n");
       fprintf(fp3, "$            absissa            ordinate\n");
       fprintf(fp3, "$#    lcid      sidr       sfa       sfo      offa      offo    dattyp\n");
       fprintf(fp3, "         1         0  1.000000  1.000000     0.000     0.000         0\n");
       fprintf(fp3, "$#                a1                  o1\n");
       fprintf(fp3, "               0.000               0.000\n");
       fprintf(fp3, "             0.45E-3      %14.3e\n",b);
     }
     else{       //NORMAL + SHEAR
       sprintf(s,"Microstructure%d/TypeBC%d/Subtype%d/AngleInc%d/LoadCurves.k",microN,j,k,l);
       fp3 = fopen(s, "w");
       fprintf(fp3, "*DEFINE_CURVE\n");
       fprintf(fp3, "$            absissa            ordinate\n");
       fprintf(fp3, "$#    lcid      sidr       sfa       sfo      offa      offo    dattyp\n");
       fprintf(fp3, "         1         0         1         1         0         0        0\n");
       fprintf(fp3, "$#                a1                  o1\n");
       fprintf(fp3, "               0.000               0.000\n");
       fprintf(fp3, "             0.45E-3      %14.3e\n",a);
       fprintf(fp3, "*DEFINE_CURVE\n");
       fprintf(fp3, "$            absissa            ordinate\n");
       fprintf(fp3, "$#    lcid      sidr       sfa       sfo      offa      offo    dattyp\n");
       fprintf(fp3, "         2         0         1         1     0.000     0.000         0\n");
       fprintf(fp3, "$#                a1                  o1\n");
       fprintf(fp3, "               0.000               0.000\n");
       fprintf(fp3, "             0.45E-3       %14.3e\n",b);
     }
   }//if normal shear on different faces

   //NORMAL-SHEAR- Normal and shear on same face
   else if(k==1 ||k==3 ||k==4 ||k==5 ||k==8 ||k==9){
      char title[20];
      sprintf(s,"Microstructure%d/TypeBC%d/Subtype%d/AngleInc%d/LoadCurves.k",microN,j,k,l);
      fp3 = fopen(s, "w");
      double e, f, g, ee, ff, gg, aa;
      aa = a;
      if(k==1){
        e=a; f=b; g=0;
      }

      if(k==3){
        e=a; f=0; g=b;
      }

      if(k==4){
        e=a; f=b; g=0;
      }
 
      if(k==5){
       e=0; f=a; g=b;
      }
  
      if(k==8){
       e=0; f=a; g=b;
      }
  
      if(k==9){
       e=a; f=0; g=b;
      }

      double magnitude = sqrt(a*a + b*b);
      if (l==5){
        char title[20];
        sprintf(s,"Microstructure%d/TypeBC%d/Subtype%d/AngleInc%d/LoadCurves.k",microN,j,k,l);
        fp3 = fopen(s, "w");
        fprintf(fp3, "*DEFINE_CURVE\n");
        fprintf(fp3, "$            absissa            ordinate\n");
        fprintf(fp3, "$#    lcid      sidr       sfa       sfo      offa      offo    dattyp\n");
        fprintf(fp3, "         1         0         1         1     0.000     0.000         0\n");
        fprintf(fp3, "$#                a1                  o1\n");
        fprintf(fp3, "               0.000               0.000\n");
        fprintf(fp3, "             0.45E-3      %14.3e\n",b);
      }
      if(l!=5){
        if(l>5){e = fabs(e);f=fabs(f);g=fabs(g);}
        fprintf(fp3, "*DEFINE_CURVE\n");
        fprintf(fp3, "$            absissa            ordinate\n");
        fprintf(fp3, "$#    lcid      sidr       sfa       sfo      offa      offo    dattyp\n");
        fprintf(fp3, "         1         0         1         1     0.000     0.000         0\n");
        fprintf(fp3, "$#                a1                  o1\n");
        fprintf(fp3, "               0.000               0.000\n");
        fprintf(fp3, "             0.45E-3          %2.4e\n", b);
        fprintf(fp3, "*DEFINE_VECTOR\n");
        fprintf(fp3, "$#     vid        xt        yt        zt        xh        yh        zh       cid\n");
        fprintf(fp3, "         1     0.000     0.000     0.000  %4.2e %4.2e  %4.2e         0\n", e,f,g);
      }
 }//Normal shear on same face

}//if j==2
fclose(fp3);
}/*end of create_load_curves function*/

/*CREATE BOUNDARY CONDITIONS*/
int boundary_conditions(int microN, int j,int k,int m, int l){
char title[100];
FILE *boundary2; 

double theta[25]={0, 0.2618, 0.5236, 0.7854, 1.0472, 1.3090, 1.5708, 1.8326, 2.0944, 2.3562, 2.618, 2.879, 3.142, 3.403, 3.665, 3.9270, 4.189, 4.451, 4.712, 4.974, 5.236, 5.498, 5.759, 6.021, 6.283};
double phi[17] = {0,0.2094,0.4189,0.6283,0.8378,1.0472,1.2566,1.4661,1.5708, 1.6755,1.8850,2.0944,2.3038,2.5133,2.7227,2.9322,3.14159};

double cosphi = fabs(cos(phi[m-1]));
if (cosphi<0.001){cosphi = round(cosphi);}
double sinphi = fabs(sin(phi[m-1]));
if (sinphi<0.001){sinphi = round(sinphi);}
double costheta = fabs(cos(theta[l-1]));
if (costheta<0.001){costheta = round(costheta);}
double sintheta = fabs(sin(theta[l-1]));
if (sintheta<0.001){sintheta = round(sintheta);}


if(j==1)
  {sprintf(title, "Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d/BoundaryConditions.k",microN,j,k,m,l);}
else{sprintf(title, "Microstructure%d/TypeBC%d/Subtype%d/AngleInc%d/BoundaryConditions.k",microN,j,k,l);}
boundary2 = fopen(title, "w");
int v1,v2,v3,v4,v5,v6,v7,v8;
int nsidx,nsidy,dofx,dofy,dofz,dofX,dofY,dofZ,nsidl, dofXX, dofYY, dofZZ, nsidm, dofxm,dofym, dofzm;
int sf, sf1, sf2;
int ns1,df1,ns2,df2,ns3,df3,ns4,df4,ns5,df5,ns6,df6, ns7,nodes,x,y,z,vid,x1,y1,z1;
vid = 1;

if(j==1 && k==1){v1=1;v2=1;v3=2;v4=1;  v5=3;v6=2;v7=4;v8=2; nsidx=2;dofx=1;dofy=0;dofz=0; nsidy=4; dofX=0;dofY=1;dofZ=0;}
if(j==1 && k==2){v1=1;v2=1;v3=2;v4=1;  v5=5;v6=3;v7=6;v8=3; nsidx=2;dofx=1;dofy=0;dofz=0; nsidy=6; dofX=0;dofY=0;dofZ=1;}
if(j==1 && k==3){v1=3;v2=2;v3=4;v4=2;  v5=5;v6=3;v7=6;v8=3; nsidx=4;dofx=0;dofy=1;dofz=0; nsidy=6; dofX=0;dofY=0;dofZ=1;}


if(j==2 && k==1){ns1=1;df1=1; ns2=2;df2=1; ns3=1;df3=2; ns4=2;df4=2; ns5=3;df5=1; ns6=4;df6=1; ns7=5;dofx=0;dofy=0;dofz=1; nodes=5;x=0;y=0;z=1;x1=1;y1=1;z1=0;}
if(j==2 && k==2){ns1=1;df1=1; ns2=2;df2=1; ns3=3;df3=3; ns4=4;df4=3; ns5=5;df5=2; ns6=6;df6=2; ns7=1;dofx=1;dofy=0;dofz=0; nodes=1;x=1;y=0;z=0;}
if(j==2 && k==3){ns1=1;df1=1; ns2=2;df2=1; ns3=1;df3=3; ns4=2;df4=3; ns5=5;df5=1; ns6=6;df6=1; ns7=3;dofx=0;dofy=1;dofz=0; nodes=3;x=0;y=1;z=0;x1=1;y1=0;z1=1;}

if(j==2 && k==4){ns1=3;df1=2; ns2=4;df2=2; ns3=1;df3=2; ns4=2;df4=2; ns5=3;df5=1; ns6=4;df6=1; ns7=5;dofx=0;dofy=0;dofz=1;nodes=5;x=0;y=0;z=1;x1=1;y1=1;z1=0;}
if(j==2 && k==5){ns1=3;df1=2; ns2=4;df2=2; ns3=3;df3=3; ns4=4;df4=3; ns5=5;df5=2; ns6=6;df6=2; ns7=1;dofx=1;dofy=0;dofz=0;nodes=1;x=1;y=0;z=0;x1=0;y1=1;z1=1;}
if(j==2 && k==6){ns1=3;df1=2; ns2=4;df2=2; ns3=1;df3=3; ns4=2;df4=3; ns5=5;df5=1; ns6=6;df6=1; ns7=3;dofx=0;dofy=1;dofz=0;nodes=3;x=0;y=1;z=0;}

if(j==2 && k==7){ns1=5;df1=3; ns2=6;df2=3; ns3=1;df3=2; ns4=2;df4=2; ns5=3;df5=1; ns6=4;df6=1; ns7=5;dofx=0;dofy=0;dofz=1;nodes=5;x=0;y=0;z=1;}
if(j==2 && k==8){ns1=5;df1=3; ns2=6;df2=3; ns3=3;df3=3; ns4=4;df4=3; ns5=5;df5=2; ns6=6;df6=2; ns7=1;dofx=1;dofy=0;dofz=0;nodes=1;x=1;y=0;z=0;x1=0;y1=1;z1=1;}
if(j==2 && k==9){ns1=5;df1=3; ns2=6;df2=3; ns3=1;df3=3; ns4=2;df4=3; ns5=5;df5=1; ns6=6;df6=1; ns7=3;dofx=0;dofy=1;dofz=0;nodes=3;x=0;y=1;z=0;x1=1;y1=0;z1=1;}

//NORMAL
if(j==1){
  //UNIAXIAL CASES
  
  //X
  if(cosphi==0 && sinphi!=0 && sintheta==0 && costheta!=0){
  //if((m==5 || m==13) && (l==1 || l==9)){
    if(sinphi*costheta>0){sf=1;}
    if(sinphi*costheta<0){sf=-1;}
        fprintf(boundary2,"*BOUNDARY_PRESCRIBED_MOTION_SET\n");
        fprintf(boundary2, "$#    nsid       dof       vad      lcid        sf       vid     death     birth\n");
        fprintf(boundary2, "         1");
        fprintf(boundary2, "         1");
        fprintf(boundary2, "         2");
        fprintf(boundary2, "         1     %5d         0  1.00E+28     0.000\n", sf);
        fprintf(boundary2, "*BOUNDARY_SPC_SET\n");
        fprintf(boundary2, "$#    nsid       cid      dofx      dofy      dofz     dofrx     dofry     dofrz\n");
        fprintf(boundary2, "         2         0         1         0         0         0         0         0\n");
        fprintf(boundary2, "*BOUNDARY_SPC_SET\n");
        fprintf(boundary2, "$#    nsid       cid      dofx      dofy      dofz     dofrx     dofry     dofrz\n");
        fprintf(boundary2, "         3         0         0         1         0         0         0         0\n");
  }

  //Y
  else if (costheta==0 && sintheta!=0 && cosphi==0 && sinphi!=0){
  //else if((m==5 || m==13) && (l==5 || l==13)){
     if(sinphi*sintheta>0){sf=1;}
     if(sinphi*sintheta<0){sf=-1;}
        fprintf(boundary2,"*BOUNDARY_PRESCRIBED_MOTION_SET\n");
        fprintf(boundary2, "$#    nsid       dof       vad      lcid        sf       vid     death     birth\n");
        fprintf(boundary2, "         3");
        fprintf(boundary2, "         2");
        fprintf(boundary2, "         2");
        fprintf(boundary2, "         1     %5d         0  1.00E+28     0.000\n", sf);
        fprintf(boundary2, "*BOUNDARY_SPC_SET\n");
        fprintf(boundary2, "$#    nsid       cid      dofx      dofy      dofz     dofrx     dofry     dofrz\n");
        fprintf(boundary2, "         4         0         0         1         0         0         0         0\n");
        fprintf(boundary2, "*BOUNDARY_SPC_SET\n");
        fprintf(boundary2, "$#    nsid       cid      dofx      dofy      dofz     dofrx     dofry     dofrz\n");
        fprintf(boundary2, "         5         0         0         0         1         0         0         0\n");
  }
  
  //Z
  else if ((sinphi==0 || costheta==0) && (sinphi==0 || sintheta==0)){
  //else if(m==1 || m==9){
     if(cosphi>0){sf=1;}
     if(cosphi<0){sf=-1;}
        fprintf(boundary2,"*BOUNDARY_PRESCRIBED_MOTION_SET\n");
        fprintf(boundary2, "$#    nsid       dof       vad      lcid        sf       vid     death     birth\n");
        fprintf(boundary2, "         5");
        fprintf(boundary2, "         3");
        fprintf(boundary2, "         2");
        fprintf(boundary2, "         1     %5d         0  1.00E+28     0.000\n", sf);
        fprintf(boundary2, "*BOUNDARY_SPC_SET\n");
        fprintf(boundary2, "$#    nsid       cid      dofx      dofy      dofz     dofrx     dofry     dofrz\n");
        fprintf(boundary2, "         6         0         0         0         1         0         0         0\n");
        fprintf(boundary2, "*BOUNDARY_SPC_SET\n");
        fprintf(boundary2, "$#    nsid       cid      dofx      dofy      dofz     dofrx     dofry     dofrz\n");
        fprintf(boundary2, "         1         0         1         0         0         0         0         0\n");
  }

  //BIAXIAL CASES
  
  //XY
  else if (cosphi==0 && sinphi!=0 && costheta!=0 && sintheta!=0){
  //else if((m==5 || m==13) && l!=1 && l!=5 && l!=9 && l!=13){
      fprintf(boundary2,"*BOUNDARY_PRESCRIBED_MOTION_SET\n");
      fprintf(boundary2, "$#    nsid       dof       vad      lcid        sf       vid     death     birth\n");
      fprintf(boundary2, "         1         1         2         1        -1         0  1.00E+28       0.0\n");
      fprintf(boundary2,"*BOUNDARY_PRESCRIBED_MOTION_SET\n");
      fprintf(boundary2, "$#    nsid       dof       vad      lcid        sf       vid     death     birth\n");
      fprintf(boundary2, "         2         1         2         1         1         0  1.00E+28       0.0\n");
      fprintf(boundary2,"*BOUNDARY_PRESCRIBED_MOTION_SET\n");
      fprintf(boundary2, "$#    nsid       dof       vad      lcid        sf       vid     death     birth\n");
      fprintf(boundary2, "         3         2         2         2        -1         0  1.00E+28       0.0\n");
      fprintf(boundary2,"*BOUNDARY_PRESCRIBED_MOTION_SET\n");
      fprintf(boundary2, "$#    nsid       dof       vad      lcid        sf       vid     death     birth\n");
      fprintf(boundary2, "         4         2         2         2         1         0  1.00E+28       0.0\n");
  }
  
  //XZ
  else if (sintheta==0 && sinphi!=0 && costheta!=0 && cosphi!=0){
  //else if((l==1 || l==9) && m!=5 && m!=13 && m!=1 && m!=9 ){
      fprintf(boundary2,"*BOUNDARY_PRESCRIBED_MOTION_SET\n");
      fprintf(boundary2, "$#    nsid       dof       vad      lcid        sf       vid     death     birth\n");
      fprintf(boundary2, "         1         1         2         1        -1         0  1.00E+28       0.0\n");
      fprintf(boundary2,"*BOUNDARY_PRESCRIBED_MOTION_SET\n");
      fprintf(boundary2, "$#    nsid       dof       vad      lcid        sf       vid     death     birth\n");
      fprintf(boundary2, "         2         1         2         1         1         0  1.00E+28       0.0\n");
      fprintf(boundary2,"*BOUNDARY_PRESCRIBED_MOTION_SET\n");
      fprintf(boundary2, "$#    nsid       dof       vad      lcid        sf       vid     death     birth\n");
      fprintf(boundary2, "         5         3         2         2        -1         0  1.00E+28       0.0\n");
      fprintf(boundary2,"*BOUNDARY_PRESCRIBED_MOTION_SET\n");
      fprintf(boundary2, "$#    nsid       dof       vad      lcid        sf       vid     death     birth\n");
      fprintf(boundary2, "         6         3         2         2         1         0  1.00E+28       0.0\n");
  }
  
  //YZ
  else if (sinphi!=0 && sintheta!=0 && cosphi!=0 && costheta==0){
  //else if((l==5 || l==13) && m!=5 && m!=13 && m!=1 && m!=9){
      fprintf(boundary2,"*BOUNDARY_PRESCRIBED_MOTION_SET\n");
      fprintf(boundary2, "$#    nsid       dof       vad      lcid        sf       vid     death     birth\n");
      fprintf(boundary2, "         3         2         2         1        -1         0  1.00E+28       0.0\n");
      fprintf(boundary2,"*BOUNDARY_PRESCRIBED_MOTION_SET\n");
      fprintf(boundary2, "$#    nsid       dof       vad      lcid        sf       vid     death     birth\n");
      fprintf(boundary2, "         4         2         2         1         1         0  1.00E+28       0.0\n");
      fprintf(boundary2,"*BOUNDARY_PRESCRIBED_MOTION_SET\n");
      fprintf(boundary2, "$#    nsid       dof       vad      lcid        sf       vid     death     birth\n");
      fprintf(boundary2, "         5         3         2         2        -1         0  1.00E+28       0.0\n");
      fprintf(boundary2,"*BOUNDARY_PRESCRIBED_MOTION_SET\n");
      fprintf(boundary2, "$#    nsid       dof       vad      lcid        sf       vid     death     birth\n");
      fprintf(boundary2, "         6         3         2         2         1         0  1.00E+28       0.0\n");
  }

  //Triaxial
  else{
      fprintf(boundary2,"*BOUNDARY_PRESCRIBED_MOTION_SET\n");
      fprintf(boundary2, "$#    nsid       dof       vad      lcid        sf       vid     death     birth\n");
      fprintf(boundary2, "         1         1         2         1        -1         0  1.00E+28       0.0\n");
      fprintf(boundary2,"*BOUNDARY_PRESCRIBED_MOTION_SET\n");
      fprintf(boundary2, "$#    nsid       dof       vad      lcid        sf       vid     death     birth\n");
      fprintf(boundary2, "         2         1         2         1         1         0  1.00E+28       0.0\n");
      fprintf(boundary2,"*BOUNDARY_PRESCRIBED_MOTION_SET\n");
      fprintf(boundary2, "$#    nsid       dof       vad      lcid        sf       vid     death     birth\n");
      fprintf(boundary2, "         3         2         2         2        -1         0  1.00E+28       0.0\n");
      fprintf(boundary2,"*BOUNDARY_PRESCRIBED_MOTION_SET\n");
      fprintf(boundary2, "$#    nsid       dof       vad      lcid        sf       vid     death     birth\n");
      fprintf(boundary2, "         4         2         2         2         1         0  1.00E+28       0.0\n");
      fprintf(boundary2,"*BOUNDARY_PRESCRIBED_MOTION_SET\n");
      fprintf(boundary2, "$#    nsid       dof       vad      lcid        sf       vid     death     birth\n");
      fprintf(boundary2, "         5         3         2         3        -1         0  1.00E+28       0.0\n");
      fprintf(boundary2,"*BOUNDARY_PRESCRIBED_MOTION_SET\n");
      fprintf(boundary2, "$#    nsid       dof       vad      lcid        sf       vid     death     birth\n");
      fprintf(boundary2, "         6         3         2         3         1         0  1.00E+28       0.0\n");
  }
}//end of j==1 for normal


//SHEAR
if(j==2){
//Uniaxial just shear
  if(l==5){
        fprintf(boundary2,"*BOUNDARY_PRESCRIBED_MOTION_SET\n");
        fprintf(boundary2, "$#    nsid       dof       vad      lcid        sf       vid     death     birth\n");
        fprintf(boundary2, "         %d", ns3);
        fprintf(boundary2, "         %d", df3);
        fprintf(boundary2, "         2");
        fprintf(boundary2, "         1         1         0  1.00E+28     0.000\n");
        fprintf(boundary2,"*BOUNDARY_PRESCRIBED_MOTION_SET\n");
        fprintf(boundary2, "$#    nsid       dof       vad      lcid        sf       vid     death     birth\n");
        fprintf(boundary2, "         %d", ns4);
        fprintf(boundary2, "         %d", df4);
        fprintf(boundary2, "         2");
        fprintf(boundary2, "         1        -1         0  1.00E+28     0.000\n");
        fprintf(boundary2,"*BOUNDARY_PRESCRIBED_MOTION_SET\n");
        fprintf(boundary2, "$#    nsid       dof       vad      lcid        sf       vid     death     birth\n");
        fprintf(boundary2, "         %d", ns5);
        fprintf(boundary2, "         %d", df5);
        fprintf(boundary2, "         2");
        fprintf(boundary2, "         1         1         0  1.00E+28     0.000\n");
        fprintf(boundary2,"*BOUNDARY_PRESCRIBED_MOTION_SET\n");
        fprintf(boundary2, "$#    nsid       dof       vad      lcid        sf       vid     death     birth\n");
        fprintf(boundary2, "         %d", ns6);
        fprintf(boundary2, "         %d", df6);
        fprintf(boundary2, "         2");
        fprintf(boundary2, "         1        -1         0  1.00E+28     0.000\n");

        fprintf(boundary2, "*BOUNDARY_SPC_SET\n");
        fprintf(boundary2, "$#    nsid       cid      dofx      dofy      dofz     dofrx     dofry     dofrz\n");
        fprintf(boundary2, "         %d         0         %d         %d         %d         0         0         0\n", ns7, dofx, dofy, dofz);
  }
  //Biaxial shear and normal
  else if(l!=5){
      //Shear not on normal face
      if(k==2 || k==6 || k==7){
        fprintf(boundary2,"*BOUNDARY_PRESCRIBED_MOTION_SET\n");
        fprintf(boundary2, "$#    nsid       dof       vad      lcid        sf       vid     death     birth\n");
        fprintf(boundary2, "         %d", ns1);
        fprintf(boundary2, "         %d", df1);
        fprintf(boundary2, "         2");
        fprintf(boundary2, "         1         1         0  1.00E+28     0.000\n");
        fprintf(boundary2,"*BOUNDARY_PRESCRIBED_MOTION_SET\n");
        fprintf(boundary2, "$#    nsid       dof       vad      lcid        sf       vid     death     birth\n");
        fprintf(boundary2, "         %d", ns2);
        fprintf(boundary2, "         %d", df2);
        fprintf(boundary2, "         2");
        fprintf(boundary2, "         1        -1         0  1.00E+28     0.000\n");
        fprintf(boundary2,"*BOUNDARY_PRESCRIBED_MOTION_SET\n");
        fprintf(boundary2, "$#    nsid       dof       vad      lcid        sf       vid     death     birth\n");
        fprintf(boundary2, "         %d", ns3);
        fprintf(boundary2, "         %d", df3);
        fprintf(boundary2, "         2");
        fprintf(boundary2, "         2         1         0  1.00E+28     0.000\n");
        fprintf(boundary2,"*BOUNDARY_PRESCRIBED_MOTION_SET\n");
        fprintf(boundary2, "$#    nsid       dof       vad      lcid        sf       vid     death     birth\n");
        fprintf(boundary2, "         %d", ns4);
        fprintf(boundary2, "         %d", df4);
        fprintf(boundary2, "         2");
        fprintf(boundary2, "         2       - 1         0  1.00E+28     0.000\n");
        fprintf(boundary2,"*BOUNDARY_PRESCRIBED_MOTION_SET\n");
        fprintf(boundary2, "$#    nsid       dof       vad      lcid        sf       vid     death     birth\n");
        fprintf(boundary2, "         %d", ns5);
        fprintf(boundary2, "         %d", df5);
        fprintf(boundary2, "         2");
        fprintf(boundary2, "         2         1         0  1.00E+28     0.000\n");
        fprintf(boundary2,"*BOUNDARY_PRESCRIBED_MOTION_SET\n");
        fprintf(boundary2, "$#    nsid       dof       vad      lcid        sf       vid     death     birth\n");
        fprintf(boundary2, "         %d", ns6);
        fprintf(boundary2, "         %d", df6);
        fprintf(boundary2, "         2");
        fprintf(boundary2, "         2        -1         0  1.00E+28     0.000\n");
        }
        //Shear on normal face
        else {
             int sf;
             if(l>5){sf=1;}
             if(l<5){sf=-1;}
             fprintf(boundary2, "*BOUNDARY_PRESCRIBED_MOTION_SET\n");
             fprintf(boundary2, "$#    nsid       dof       vad      lcid        sf       vid     death     birth\n");
             fprintf(boundary2, "         %d         4         2         1        %2d         %d      1E28     0.000\n", ns1, sf, vid);
             fprintf(boundary2, "*BOUNDARY_SPC_SET\n");
             fprintf(boundary2, "$#    nsid       cid      dofx      dofy      dofz     dofrx     dofry     dofrz\n");
             fprintf(boundary2, "         %d         0         %d         %d         %d         0         0         0\n", nodes, x, y, z);
             fprintf(boundary2, "*BOUNDARY_SPC_SET\n");
             fprintf(boundary2, "$#    nsid       cid      dofx      dofy      dofz     dofrx     dofry     dofrz\n");
             fprintf(boundary2, "         %d         0         %d         %d         %d         0         0         0\n", ns2, x1, y1, z1);
       }//shear on same face as normal
   }//l doesn't equal five
}//j==2  
fclose(boundary2);

}//end boundary_conditions_NN function

/*READ NODES AND ELEMENTS FROM FILE*/
int read_nodesandelements(int microN){
FILE *fp1;
int i;
char s[50];
char tmp[100];
//microN = 3;
sprintf(s,"NewMicro%d.k",microN);
printf("Reading %s\n",s);
fp1 = fopen(s, "r");
int flag =0;
/* read file until hit nodes part of file */
while(fp1!=NULL && flag==0){
     fscanf(fp1,"%s",&tmp);
     if(strcmp(tmp,"*NODE")==0){
        flag =1;
     }
}
/* determine number of nodes and write to array */
flag = 0;
i = 0;
int counter=0;
int d1,d2,d3;
int  sizenodes;
double f1, f2, f3;
coordinates = (double*)malloc(ndim*1*sizeof(double));
nnodes = 0;
for (i=0;i<7;i++){fscanf(fp1,"%s",&tmp);}
while(fp1!=NULL && flag==0){
     fscanf(fp1,"%s",&tmp);
     if(strcmp(tmp,"$")==0){
       flag=1;
       nnodes = d1; 
     } //end of if
     else{
       d1=atoi(tmp);
       fscanf(fp1,"%lf",&f1);
       fscanf(fp1,"%lf",&f2);
       fscanf(fp1,"%lf",&f3);
       fscanf(fp1,"%d", &d2);
       fscanf(fp1,"%d",&d3);
       coordinates=(double*)realloc(coordinates,ndim*d1*sizeof(double));
       coordinates[ndim*(d1-1)+0]=f1*5E-5;
       coordinates[ndim*(d1-1)+1]=f2*5E-5;
       coordinates[ndim*(d1-1)+2]=f3*5E-5;
      } //end of else
} //end of while loop
flag =0;

/*Find elements part of file*/
while(fp1!=NULL && flag==0){
     fscanf(fp1,"%s",&tmp);
     if(strcmp(tmp,"*ELEMENT_SOLID")==0){
       flag =1;
     }//end if 
}//end while

/*Scan and write elements to array*/
int e1, e2, e3, e4, e5, e6, e7, e8, e9, e10;
elements = (int*)malloc(9*sizeof(int));
flag = 0;
while(fp1!=NULL && flag==0){
     fscanf(fp1,"%s",&tmp);
     if(strcmp(tmp,"*ELEMENT_MASS")==0){
       flag=1;       
       num_elements=e1;
     } //end of if
     else{
       e1=atoi(tmp);
       fscanf(fp1,"%d",&e2);
       fscanf(fp1,"%d",&e3);
       fscanf(fp1,"%d",&e4);
       fscanf(fp1,"%d",&e5);
       fscanf(fp1,"%d",&e6);
       fscanf(fp1,"%d",&e7);
       fscanf(fp1,"%d",&e8);
       fscanf(fp1,"%d",&e9);
       fscanf(fp1,"%d",&e10);
       elements=(int*)realloc(elements,9*e1*sizeof(int));
       elements[(e1-1)*9+0]=e1;
       elements[(e1-1)*9+1]=e3;
       elements[(e1-1)*9+2]=e4;
       elements[(e1-1)*9+3]=e5;
       elements[(e1-1)*9+4]=e6;
       elements[(e1-1)*9+5]=e7;
       elements[(e1-1)*9+6]=e8;
       elements[(e1-1)*9+7]=e9;
       elements[(e1-1)*9+8]=e10;
      }//end of else
} //end of while 
num_elements = e1;
printf("The number of elements = %d\n", num_elements);

fclose(fp1);
}//end of read_nodeselements function

/*WRITE NODES AND ELEMENTS*/
int write_nodeselements(int microN, int aa, int bb, int mm, int cc){
FILE *nodeselts;
char s[100];
int i;
if(aa==1)
  {sprintf(s,"Microstructure%d/TypeBC%d/Subtype%d/PhiInc%d/AngleInc%d/NodesElements.k",microN, aa, bb,mm, cc);}
else{sprintf(s,"Microstructure%d/TypeBC%d/Subtype%d/AngleInc%d/NodesElements.k",microN, aa, bb, cc);}
nodeselts = fopen(s, "w");
double edge_lengthx = max_x-min_x;
double scalingx = 3.15E-3/edge_lengthx;
double edge_lengthy = max_y-min_y;
double scalingy = 3.15E-3/edge_lengthy;
double edge_lengthz = max_z-min_z;
double scalingz = 3.15E-3/edge_lengthz;


/*Print nodes part of file*/
fprintf(nodeselts, "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
fprintf(nodeselts, "$               Writing Nodes\n");
fprintf(nodeselts, "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
fprintf(nodeselts, "$\n");
fprintf(nodeselts, "*NODE\n");
fprintf(nodeselts, "$   Node               x               y               z      tc      rc\n");

for (i=0;i<nnodes;i++){
    //fprintf(nodes,"%8d %15.5f %15.5f %15.5f       0       0 \n",i+1,coordinates[ndim*i+0],coordinates[ndim*i+1],coordinates[ndim*i+2]);
    fprintf(nodeselts,"%8d  %14.5e  %14.5e  %14.5e       0       0 \n",i+1,coordinates[ndim*i+0]*scalingx,coordinates[ndim*i+1]*scalingy,coordinates[ndim*i+2]*scalingz);
}

/*Print elements part of file*/
fprintf(nodeselts, "$\n");
fprintf(nodeselts, "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
fprintf(nodeselts, "$               Writing Elements\n");
fprintf(nodeselts, "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
fprintf(nodeselts, "$\n");
fprintf(nodeselts, "*ELEMENT_SOLID\n");

for (i=0;i<num_elements;i++){
    fprintf(nodeselts, "%8d       1\n", elements[9*i+0]);
    fprintf(nodeselts,"%8d%8d%8d%8d%8d%8d%8d%8d\n",elements[9*i+1],elements[9*i+2],elements[9*i+3],elements[9*i+4],elements[9*i+5],elements[9*i+6],elements[9*i+7],elements[9*i+8]);
}
fprintf(nodeselts, "$\n");

fclose(nodeselts);
}//end of write_nodeselements function
