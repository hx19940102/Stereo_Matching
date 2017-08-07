# Stereo_Matching

Based on the result of Jure Žbontar, Yann LeCun: [Stereo Matching by Training a Convolutional Neural Network to Compare Image Patches](https://arxiv.org/abs/1510.05970), I further improve the quality and the accuracy of the disparity map calculated by convolutional neural network based on my own algorithm.

For details about stereo vision and stereo matching methods, please see the [PowerPoint slides](https://github.com/hx19940102/Stereo_Matching/blob/master/Presentation_12171016_Xiao%20Huang.pptx)

# Further improvement algorithm for Disparity Map
Inspired by Cross-based matching cost aggregation, I suggest a new algorithm for improving the accuracy of edge region and low texture region of disparity map. ：

1. Based on cross-based growth, set threshold for growth and perform both vertical and horizontal growth to get a neighboring growth region for every point of the disparity image. 

2. Based on neighboring growth region, build a disparity histogram for this region.

3. Based on disparity histogram, select the disparity with highest frequency to replace the disparity of original point.

4. For every point on disparity map, repeat step 1,2,3 until we update the whole disparity map.

# The result of the improved disparity map
Original Image:

![alt text](https://github.com/hx19940102/Stereo_Matching/blob/master/images/chair.jpg)

Original Depth Map:

![alt text](https://github.com/hx19940102/Stereo_Matching/blob/master/images/chair_res_ori.jpg)

Improved Depth Map:

![alt text](https://github.com/hx19940102/Stereo_Matching/blob/master/images/chair_res_improved.jpg)
