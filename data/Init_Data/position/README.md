# datasets

## 1. Title of dataset
 Position (Positioning data).

## 2. Name of dataset creator
IDEKO 

## 3. Description of data
Positioning data is collected and analysed in order to quantify the degradation of the components that are relates to axis, that is, motors, ball screw, ball nut, etc. Data is collected while the machine is moving.

This case is focused on the errors related to the positioning of the ball nut, as these kind of errors may cause high precision failures. For this purpose, the most interesting indicators are the linear and angular position of the axis.  In this case, periodic movements in a forward-backward way are executed, so an unique axis in analysed in each batch data.

On the one hand, the angular position of the axis is a signal obtained from the motors. It represents the position of the axis according to the rotatory movements the motor executes. On the other hand, the linear position of the axis is a signal obtained externally, hence it is considered as the real position of the axis. The difference between these two signals may be caused, for example, from backlash errors in the ball nut.

Backlash is the physical phenomena caused by the clearance between the ball bearing elements and the screw and nut tracks, resulting in lost motion in the screw. Therefore, the variable that is obtained by the difference of the two previously mentioned position measures can characterize the backlash errors between the screw and the ball nut.

The variable analysed (backlash) is obtained by the difference between the angular and linear position of the first motor (f1 and f2). So the variables monitorized are the time and the backlash.

## 4. Source of data
The monitoring of this data is done in higher sampling frequency, as these failures are harder to track in usual sampling rates. Data is sent in batches, periodically, in order to capture the wear of the components along the time.

## 5. Creation date
These datasets where created on December 2022.

## 6. Format
Data is stored in csv format.




