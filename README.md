# STM32F407-MG996R-AND-GUI-TESTER
STM32F407 with PWM to control MG996R, and GUI tester for MAX OS X
</br>
1.</br>
make STM32F407 project, and burn to board.(0.5ms~2.5ms)</br>
</br>
2.</br>
build GUI</br>
> g++ -o2 -o wxservotester.app wxservotester.cpp serialport.cpp connectargsdlg.cpp `wx-config --cxxflags --libs`</br>
3.</br>
wxservotester.app -> Tools -> ConnectDevice</br>
4.</br>
Test your servo motor!</br>
</br>
![alt text](https://github.com/GCY/STM32F407-MG996R-AND-GUI-TESTER/blob/master/gui.png?raw=true)
</br>
</br>
</br>
[![Audi R8](https://youtu.be/wf9-RlVoMTk/0.jpg)](https://youtu.be/wf9-RlVoMTk)
</br>
</br>
Dependency： </br>
wxWidgets 3.0 : https://github.com/wxWidgets </br>
POSIX API </br>
</br>
