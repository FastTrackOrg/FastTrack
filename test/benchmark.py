from accuracyTest import *
from scipy.optimize import differential_evolution

"""
This script can be used to benchmark FastTrack and find the best set of parameters
for the test image sequence.
"""

def f(x, reference):
  tracking("dataSet/", "images/", normDist=x[0], normAngle=x[1], maxDist=x[2], maxTime=x[3], normArea=x[4], normPerim=x[5])
  trackingData = pandas.read_csv("dataSet/images/Tracking_Result/tracking.txt", engine="python", sep="\t", usecols=['xBody', 'yBody', 'imageNumber', 'id'])              
  res = errorsCounter(reference, trackingData)
  shutil.rmtree("dataSet/images/Tracking_Result/")
  return res[1]

bounds = [(0, 400), (0, 180), (5, 400), (5, 200), (0, 100), (0, 100)]
reference = pandas.read_csv("dataSet/images/Groundtruth/Tracking_Result/tracking.txt", engine="python", sep="\t", usecols=['xBody', 'yBody', 'imageNumber', 'id'])
res = differential_evolution(f, bounds, args=[reference], maxiter=4000)
print(res)
