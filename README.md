## Instructions for Analyzing Yield Point Data

The main location of the Matlab analysis code that you’ll need is: 

```
/group/Allison/Data/MatlabCode/YieldPoints
'''

In this YieldPoints directory, there is a directory for
1.	Separate micro data- these are the individual yield points/outputted files for each of the microstructures. I was originally using this directory as backup, but all the files you should need are in the second directory, Combined Data
2.	CombinedData- This has all of the files necessary for getting the averages, SD, and optimization

In the CombinedData Directory: 

1.	Archives- random old files
2.	Figures- Some figures that I had made, although, these will need to be updated with new ones most likely
3.	3DAverages- Yield points averaged in 3D space
4.	BiaxialAverages- Averages between all the micros for biaxial loading cases (XY, YZ and XZ)
5.	Optimization- all the code necessary for creating the full yield surface

Within the 3DAverages and BiaxialAverages directories, there are directories for stress and for strain. Each of these directories contains essentially the same info/code/files, but just altered to be specific to stress or strain- for both of them, the code is identical.

Instructions for use: 

~/3DAverages/Stress(or Strain)
In this directory, you’ll find 30 csv files. Each of these files comes from a single microstructure. Within the csv files are 401 coordinates representative of the yield points from a specific simulation. The AveragedSurfaceStresses.m file takes the average between each of the 30 micros to create an averaged surface of which it outputs 2 3D plots- one with just the averaged points, and a second with all the micros superimposed as well as the average (this one is kind of messy to see). This file has a copy and past of the SD portion of the code from the biaxial portion. It’s not currently set up for 3D which is something you will have to do if you want a plus and minus SD pointset to go along with the average points. The Strain directory is identical, but uses strain files instead of stresses. The output file, StressAverages.csv, will then be the input csv file into the optimization code that I’ll describe later. You’ll need to copy this file there, as I don’t have it set up to write the file to the Optimization directory.

~/BiaxialAverages/Stress (or Strain)
In this directory, you’ll find .dat files and Matlab script that averages the biaxial stresses as well as calculates the SD.

For each microstructure, there are 3 files, one for each of the biaxial cases- Within these files there are coordinates for yield points from those specific simulations. Based on the way that I set up the boundary conditions, the number of points in the XY, YZ and XZ simulations are not identical. This is no issue for you because I already set it up properly in the code, but just something to take note of. So each micro gets 3 .dat files specific to the yield points from XY, YZ and XZ boundary condition simulations. For each of the 3 biaxial cases, the Matlab script, BiaxialStresses_andStandardDeviation.m, averages all the points between each microstructure. It also calculates the standard deviation from the mean for each of the cases. This is then added and subtracted from the mean to create the “fuzzy” yield envelope that we desire. I had to write a lot of “if” statements to make sure this plotted properly. Depending on the sign (+/-) of each of the coordinates, the SD was added or subtracted from the mean, so that we got one set of SD points inside the average, as well as one set of SD points that was outside the average. I have done this for these biaxial plots, but like I said above, I have not written this out for the 3D plots. It can be done similarly to what I did here, but you’ll have to account for a third dimension. If you need help with this or don’t understand what I’m talking about, let me know and I can help.  

The result fro the script is an average of points for XY, YZ and XZ loading conditions. The script produces 3 plots for these. Each plot has the individual microstructure yield points, as well as the calculated average. The script also outputs the SD plots, and also plots the XY, YZ and XZ plots superimposed to show isotropy in results. In addition to visual plots, the script outputs the data into files. The averages for each of the biaxial loading condition cases get saved in the BiaxialAverages directory. Similarly, the SD data files get put in the SDInfo directory. Nothing gets saved to the Images directory, but that’s just where I put some of the images to help organize. These will need to be updated with the most recent images that are produced from the scripts. 

~/CombinedData/Optimization
In here, I have the code and data to do the optimization. 4 files are necessary for this

1.	SurfaceOptimization.m- This is the main code that you will open and run. The other three just need to be in the same directory.
2.	StressAverages.csv- This is the output file from the AveragedSurfaceStresses.m code in the 3DAverages directory. This is the average of all the 3D yield points from all 30 micros. This file serves as the input for the above code.
3.	Coefficiants.m- This contains all the coefficiants for the complex equations that I have for the yield surface. Although I’m calling this “optimization”, no real optimization is happening in these codes. I had used Matlab optimization to optimize the coefficients to minimize the error between the output yield surface and the input yield points. However, this didn’t seem to work really well and only served as a good starting point for determining the general ballpark of the coefficients. What I mainly did with this coefficiants code was play around with the numbers until the plotted surface matched well with the yield points. This is definitely not accurate, but was the best I could come up with. As an output of the SurfaceOptimization.m code, I included the residuals as somewhat of a gauge of how good the fit was. You’ll have to play around with the current configuaration, because the values for the coefficients I have in there now are old, and only accurate for the averages for micros 1-20. At the time, I hadn’t yet extracted data for micros 21-30, and it seems that these last micros drastically changed the shape of the average yield points.
4.	Ezimplot3.m-This is just a function that allows you to plot 3D implicit plots (what shows up in red after running the script). I found this online, and you should never have to change anything in it. 




### Markdown

Markdown is a lightweight and easy-to-use syntax for styling your writing. It includes conventions for

```markdown
Syntax highlighted code block

# Header 1
## Header 2
### Header 3

- Bulleted
- List

1. Numbered
2. List

**Bold** and _Italic_ and `Code` text

[Link](url) and ![Image](src)
```

For more details see [GitHub Flavored Markdown](https://guides.github.com/features/mastering-markdown/).

### Jekyll Themes

Your Pages site will use the layout and styles from the Jekyll theme you have selected in your [repository settings](https://github.com/PSUCompBio/BoneMicroSim/settings). The name of this theme is saved in the Jekyll `_config.yml` configuration file.

### Support or Contact

Having trouble with Pages? Check out our [documentation](https://help.github.com/categories/github-pages-basics/) or [contact support](https://github.com/contact) and we’ll help you sort it out.
