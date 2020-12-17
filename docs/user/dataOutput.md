---
id: dataOutput
title: Tracking Result
sidebar_label: Tracking Result
---

After a tracking analysis (or an analysis preview), FastTrack saves several files inside the **Tracking_Result** folder located inside the image sequence folder or inside the **Tracking_Result_VideoFileName** for a video file:

* *tracking.txt*: the tracking result
* *annotation.txt*: the annotation
* *background.pgm*: the background image
* *cfg.toml*: the parameters used for the tracking

The tracking result file is simply a text file with 20 columns separated by a '\t' character. This file can easily be loaded to subsequent analysis see [this example](http://www.fasttrack.sh/UserManual/blog/2019/06/21/Data-analysis-python/).

* **xHead, yHead, tHead**: the position (x, y) and the absolute angle of the object's head.
* **xTail, yTail, tTail**: the position (x, y) and the absolute angle of the object's tail.
* **xBody, yBody, tBody**: the position (x, y) and the absolute angle of the object.
* **headMajorAxisLength, headMinorAxisLength, headExcentricity**: parameters of the head's ellipse (headMinorAxisLength and headExcentricity are semi-axis length).
* **bodyMajorAxisLength, bodyMinorAxisLength, bodyExcentricity**: parameters of the body's ellipse (bodyMinorAxisLength and bodyExcentricity are semi-axis length).
* **tailMajorAxisLength, tailMinorAxisLength, tailExcentricity**: parameters of the tail's ellipse (bodyMinorAxisLength and bodyExcentricity are semi-axis length).
* **imageNumber**: index of the frame.
* **id**: object unique identification number.

<div style="overflow-x:auto;">
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }

    .dataframe tbody tr th {
        vertical-align: top;
    }

    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>xHead</th>
      <th>yHead</th>
      <th>tHead</th>
      <th>xTail</th>
      <th>yTail</th>
      <th>tTail</th>
      <th>xBody</th>
      <th>yBody</th>
      <th>tBody</th>
      <th>curvature</th>
      <th>...</th>
      <th>headMinorAxisLength</th>
      <th>headExcentricity</th>
      <th>tailMajorAxisLength</th>
      <th>tailMinorAxisLength</th>
      <th>tailExcentricity</th>
      <th>bodyMajorAxisLength</th>
      <th>bodyMinorAxisLength</th>
      <th>bodyExcentricity</th>
      <th>imageNumber</th>
      <th>id</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>515.733</td>
      <td>181.263</td>
      <td>3.086240</td>
      <td>577.710</td>
      <td>191.101</td>
      <td>2.801250</td>
      <td>545.303</td>
      <td>185.957</td>
      <td>2.961370</td>
      <td>2.856190e-04</td>
      <td>...</td>
      <td>11.7597</td>
      <td>0.937833</td>
      <td>46.3947</td>
      <td>9.54795</td>
      <td>0.978594</td>
      <td>74.3017</td>
      <td>12.2626</td>
      <td>0.986287</td>
      <td>0</td>
      <td>0</td>
    </tr>
    <tr>
      <th>1</th>
      <td>723.060</td>
      <td>167.722</td>
      <td>3.204620</td>
      <td>789.967</td>
      <td>163.884</td>
      <td>3.232200</td>
      <td>755.204</td>
      <td>165.879</td>
      <td>3.205630</td>
      <td>6.985190e-06</td>
      <td>...</td>
      <td>13.8909</td>
      <td>0.928450</td>
      <td>50.3148</td>
      <td>10.80170</td>
      <td>0.976684</td>
      <td>80.1641</td>
      <td>12.5170</td>
      <td>0.987735</td>
      <td>0</td>
      <td>1</td>
    </tr>
    <tr>
      <th>2</th>
      <td>506.872</td>
      <td>178.392</td>
      <td>2.991110</td>
      <td>570.443</td>
      <td>188.991</td>
      <td>2.945870</td>
      <td>537.073</td>
      <td>183.428</td>
      <td>2.972330</td>
      <td>6.621030e-05</td>
      <td>...</td>
      <td>11.7005</td>
      <td>0.940189</td>
      <td>47.2875</td>
      <td>8.63518</td>
      <td>0.983185</td>
      <td>76.3063</td>
      <td>10.3726</td>
      <td>0.990718</td>
      <td>1</td>
      <td>0</td>
    </tr>
    <tr>
      <th>3</th>
      <td>721.137</td>
      <td>168.071</td>
      <td>3.196860</td>
      <td>787.819</td>
      <td>164.195</td>
      <td>3.233310</td>
      <td>753.288</td>
      <td>166.202</td>
      <td>3.205520</td>
      <td>2.174710e-05</td>
      <td>...</td>
      <td>13.9579</td>
      <td>0.927092</td>
      <td>49.7841</td>
      <td>10.92570</td>
      <td>0.975621</td>
      <td>79.8045</td>
      <td>12.6034</td>
      <td>0.987451</td>
      <td>1</td>
      <td>1</td>
    </tr>
    <tr>
      <th>4</th>
      <td>497.370</td>
      <td>176.719</td>
      <td>3.037370</td>
      <td>561.544</td>
      <td>188.657</td>
      <td>2.922960</td>
      <td>527.723</td>
      <td>182.365</td>
      <td>2.959540</td>
      <td>9.497620e-05</td>
      <td>...</td>
      <td>11.4143</td>
      <td>0.946052</td>
      <td>47.3322</td>
      <td>8.87084</td>
      <td>0.982280</td>
      <td>77.1759</td>
      <td>10.4940</td>
      <td>0.990712</td>
      <td>2</td>
      <td>0</td>
    </tr>
    <tr>
      <th>5</th>
      <td>719.335</td>
      <td>168.806</td>
      <td>3.200110</td>
      <td>787.030</td>
      <td>165.102</td>
      <td>3.232640</td>
      <td>751.426</td>
      <td>167.050</td>
      <td>3.203170</td>
      <td>1.624720e-05</td>
      <td>...</td>
      <td>13.9775</td>
      <td>0.930459</td>
      <td>50.3214</td>
      <td>10.79590</td>
      <td>0.976716</td>
      <td>80.9190</td>
      <td>12.5916</td>
      <td>0.987819</td>
      <td>2</td>
      <td>1</td>
    </tr>
  </tbody>
</table>
</div>

Positions are in pixels, in the frame of reference of the original image, zero is in the top left corner. Lengths and areas are in pixels. Angles are in radians in the intervale 0 to 2*pi.

    0  --  > x  
    ¦  
    v  
    y  

**Note:** If several tracking analyses are performed on the same image sequence, the previous folder is not erased. It will be renamed as **Tracking_result_DateOfTheNewAnalysis**.

## Data analysis

The tracking file can be opened for subsequent analysis:
```python
# Python
data = pandas.read_csv("tracking.txt", sep='\t')
```

```julia
# Julia
using CSV
CSV.read("tracking.txt", delim='\t')
```

```R
# R
read.csv("tracking.txt", header=T sep="\t")
```
