# Setup

## Windows setup

### IDE setup
We strongly advise using **Visual Studio Code**, which can be downloaded [here](https://code.visualstudio.com/). * 

### Git setup
You will need Git to contribute to this project. While the client is optional, if you are going to use **GitHub Desktop**, it is recommendable that you install the Git bash too so you can perform git command in case of need.

You can download the Git installer for Windows [here](https://git-scm.com/download/win). *

### GitHub Desktop setup (optional but recommended)
You can download the GitHub Desktop for Windows [here](https://desktop.github.com/).*

### Arduino setup

You can download Arduino for Windows directly from [here](https://www.arduino.cc/en/Main/Software). *

### Teensyduino setup

You will also need to install **Teensyduino**, which can be downloaded [here](https://www.pjrc.com/teensy/td_download.html). *

*Just download and install the latest version for your Windows.

## Project configuration
### Cloning the repository
If you installed **GitHub Desktop**, you could directly clone the repository by going to "**File** > **Clone repository**" or by hitting "**Ctrl + Shift + O**".

![Contextual image of repository clone](Images/Setup/clone-repository-context-windows.jpg)

### Visual Studio Code configuration

Once you have the repository cloned into your machine, we will open it with **Visual Studio Code**.

To do so, just open Visual Studio Code, go to "**File** > **Open Folder**" or just hit "**Ctrl + K**" or "**Ctrl + O**".

Then, look for the folder containing your local version of the repository.

You should have something like this:

![Visual Studio Code Welcome for Windows](Images/Setup/visual-studio-code-welcome-windows.jpg)

The first thing that you need to do is to use your own **c_cpp_properties.json** file. In the folder **.vscode**, you will find two (2) demo files.

In this case, you want to duplicate the **c_cpp_properties.windows.sample.json** file and name it as **c_cpp_properties.json**.
Please, keep in mind that those json files are mere **boilerplate** files, so you might end-up replacing those with your **System setup configuration**.

You should end up having three (3) files under the **.vscode** folder:

![.vscode file list](Images/Setup/vscode-file-list.jpg)

As you can see, the **c_cpp_properties.json** file appears in "gray". That means that it is ignored by Git, as stated in the **.gitignore** file in the project. That means that you can make any modifications to that file that will not be affected by the code versions.

![c_cpp_properties for Windows](Images/Setup/vscode-cpp-file-windows.jpg)

Once you are there, you will need some extensions that will help you with your codding.

To do so, you can click on "**View** > **Extensions**" or just hit "**Ctrl + Shift + X**".

Look for the **Arduino** Extension and the **C/C++** Extension. The extension **PlatformIO IDE** is not required, but you might end up installing that if you want to understand better how Arduino works.

![Extensions to install in VSCode](Images/Setup/vscode-extensions-to-install.jpg)

Now you should be able to code without issues for the project.

### Arduino IDE configuration

Now we will set up the project in the **Arduino IDE** so that we can **Compile** / **Verify** and **Deploy** the project into the **Polaris**.

Just open the **Arduino IDE**.
Then go to "**File** > **Open**" or hit "**Ctrl + O**" and navigate to your **project folder** > **"StarCoreEntryPoint"** and open the file **StarCoreEntryPoint.ino**.

Now we need to configure the project so it actually can be built for our board.

Go to "**Tools**" and:

1. Set **Board** to "**Teensy 3.2 / 3.1**.
2. Set **USB Type** to **USB**.
3. Set **CPU Speed** to **72 Mhz**.
4. Set **Optimize** to **Faster**.
5. Set **Keyboard Layout** to **US English**.
6. If you have connected the **Polaris** to your computer via USB, you can also pick the **COM port** where it is connected on **Port**.

Now we need to gather the custom libraries that we are building in this project. 

Go to "**File** > **Preferences**" or hit "**Ctrl + ,**" and inside of Sketchbook location, place the location of your **project folder** and press **OK**.

Now, if you go to "**File** > **Sketchbook**", you should see **StarCoreEntryPoint** and **StartCoreLoader**. That only means that the libraries are available for the compiler.

Now you can click on the "**Check**" symbol to **verify** the project by building it.

The first time you build the project, you will probably see warnings related to external libraries we are using. Don't mind those as they are only warnings and won't break the compilation.

Once you can see the message "**Done compiling** you are good to go.

To push the code to your **Polaris**, you will need to press the **Arrow** symbol to **Upload** the code to the board. 

There will be a noise produced by your **Polaris** and your computer (disconnection and connection of a USB device sound) that indicates that the firmware has been uploaded to the **Polaris**.

