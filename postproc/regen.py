
# stdlib
import os
import numpy as np      # Configure seed

# ourlib
from pytools import Reader      # Parse training/testing files
from pytools import Plot


mseList = []
YTrainPreds = []
YTestPreds = []

i = 1
for filename in os.listdir("out/"):

    if filename.endswith(".Train.csv"):

        filename.replace(".csv",".png")
        filename = f"Output/{filename}"
        Xs, Y, N, dY, W, M = Reader.CsvDatasetFile(filename)
        
        yRange = np.max(Y)-np.min(Y)
        xRange = np.max(Xs[:,0])-np.min(Xs[:,0])
        extra = 0.15

        Plot.Compare(filename, Xs[:,0], Y, M, title = f"Iteration {filename[10:filename.find('.')]}", num = i, minY = np.min(Y)-yRange*extra, maxY = np.max(Y)+yRange*extra, minX = np.min(Xs[:,0])-xRange*extra, maxX = np.max(Xs[:,0])+xRange*extra)
        i += 1