# SETUP

## Set up on OSX

### Prerequisites

Make sure you have these installed on your system.

1. XCode
2. Homebrew or MacPorts
3. GCC
4. Git

If you don't have MacPorts installed, just download it from [their website](https://www.macports.org/install.php) and run its setup.
If you don't have Brew or Git installed, refer [this](http://www.pyladies.com/blog/Get-Your-Mac-Ready-for-Python-Programming/).
If you don't have GCC installed, use xcode to install it by following command:

```commandline
xcode-select --install
```

You can also install GCC by following [this](https://www.mkyong.com/mac/how-to-install-gcc-compiler-on-mac-os-x/).

Change directory to the location where you want the Oracc repository and do:

```commandline
git clone https://github.com/oracc/oracc.git
```
	
This creates a repo 'oracc'.  Change directory to the repo:

```commandline
cd oracc
```

### Steps

Note there are two ways to do the package setup, either you can run [pkg-macos-brew](pkg-macos-brew.sh), 
if you want to use brew as your package manager or you can run [pkg-mac-osx](pkg-mac-osx.sh), if you use
macports for package installations. 

I am a brew user though so following steps are for [pkg-macos-brew](pkg-macos-brew.sh).
The steps and shell files are similar in both of them. I would rather suggest to use [pkg-mac-osx](pkg-mac-osx.sh) as
macport has msort formulae, which is missing from brew repository. 

1. Now, you need to run [pkg-macos-brew](pkg-macos-brew.sh) to get the basic packages for the project.
Please run that by the following command:

    ```commandline
    ./pkg-macos-brew
    ```
    In case, if it fails on line 3 giving this error:

    ```txt
    /pkg-macos-brew.sh: line 3: .profile: No such file or directory
    ```

    Then, comment the line 3 with # as that profile is specific to owner's repository structure.

    Finally, expect an output containing text like this.

    ```txt
    ==> Downloading https://homebrew.bintray.com/bottles/autoconf-2.69.sierra.bottle
    ==> Summary
    🍺  /usr/local/Cellar/autoconf/2.69: 70 files, 3.0MB
    ==> Downloading https://homebrew.bintray.com/bottles/automake-1.15.1.sierra.bott
    🍺  /usr/local/Cellar/automake/1.15.1: 131 files, 3.0MB
    ==> Downloading https://homebrew.bintray.com/bottles/libtool-2.4.6_1.sierra.bott
    In order to prevent conflicts with Apple's own libtool we have prepended a "g"
    so, you have instead: glibtool and glibtoolize.
    ==> Summary
    🍺  /usr/local/Cellar/libtool/2.4.6_1: 70 files, 3.7MB
    Warning: gdbm 1.13 is already installed
    ==> Downloading https://homebrew.bintray.com/bottles/pcre-8.41.sierra.bottle.tar
    🍺  /usr/local/Cellar/pcre/8.41: 204 files, 5.3MB
    Error: No available formula with the name "msort"
    Error: No similarly named formulae found.
    Error: No formulae found in taps.
    ==> Downloading https://homebrew.bintray.com/bottles/xmlrpc-c-1.39.12.sierra.bot
    🍺  /usr/local/Cellar/xmlrpc-c/1.39.12: 110 files, 1.9MB
    ==> Installing dependencies for rnv: expat
    ==> Installing rnv dependency: expat
    ==> Downloading https://homebrew.bintray.com/bottles/expat-2.2.4.sierra.bottle.t
    ==> Pouring expat-2.2.4.sierra.bottle.tar.gz
    ==> Caveats
    This formula is keg-only, which means it was not symlinked into /usr/local,
    because macOS includes Expat 1.5.
    
    If you need to have this software first in your PATH run:
      echo 'export PATH="/usr/local/opt/expat/bin:$PATH"' >> ~/.bash_profile
    
    For compilers to find this software you may need to set:
        LDFLAGS:  -L/usr/local/opt/expat/lib
        CPPFLAGS: -I/usr/local/opt/expat/include
    For pkg-config to find this software you may need to set:
        PKG_CONFIG_PATH: /usr/local/opt/expat/lib/pkgconfig
    
    ==> Summary
    🍺  /usr/local/Cellar/expat/2.2.4: 18 files, 429.8KB
    ==> Installing rnv
    ==> Downloading https://homebrew.bintray.com/bottles/rnv-1.7.11.sierra.bottle.ta
    🍺  /usr/local/Cellar/rnv/1.7.11: 9 files, 467.8KB
    ==> Downloading https://homebrew.bintray.com/bottles/tre-0.8.0.sierra.bottle.tar
    🍺  /usr/local/Cellar/tre/0.8.0: 16 files, 156.0KB
    ==> Downloading https://homebrew.bintray.com/bottles/utf8proc-2.1.sierra.bottle.
    🍺  /usr/local/Cellar/utf8proc/2.1: 9 files, 623.5KB
    ==> Downloading https://homebrew.bintray.com/bottles/gmp-6.1.2.sierra.bottle.1.t
    ```

2. Few things to note from above installation are missing msort formulae from brew repository, 
conflicting libtool installation and conflicting Expat version. 

    To resolve missing msort formulae, manually install msort from [its website](http://billposer.org/Software/msort.html).


3. To resolve Expat version issue, use the higher version Expat installed from brew by following command.

    ```commandline
    echo 'export PATH="/usr/local/opt/expat/bin:$PATH"' >> ~/.bash_profile
    ```
    
    Restart the shell.
    
4. Create a file oraccenv.sh with similar structure to [this example](oraccenv.example).
   This file will set the necessary environment variables. Refer [this](ORACC-INSTALL) for their meaning.
   
   ```commandline
    ORACC ORACC_BUILDS ORACC_HOME ORACC_MODE ORACC_USER ORACC_HOST
    ```
   Make sure these locations don't require admin privilages (sudo) to write on them.
   They should be owned by current user.
   
5. Run the following command:

    ```commandline
    sudo ./preconfig.sh
    ```   
    In the end, make sure your environment has these pre-configured variables by doing echo.
    
    For selective installation of the atf-checker, only following command will be sufficient from [the file](preconfig.sh) 
    to create the directories at the position specified in oraccenv.sh.
    
    ```commandline
    ./oraccenv.sh
    mkdir -p $ORACC
    mkdir -p $ORACC_BUILDS/tmp/sop
    ```

6. After pre-configuration, change line 9 of the [mac-config.sh](mac-config.sh) file to an existing directory where you have write permissions by specifying its absolute path.
   This directory will serve as the Installation Directory ($Installation Directory). 

    Then, run the following command:

    ```commandline
    ./mac-config.sh
    ```
    
    In case, if it complains about automake unable to create Makefile.in or ./compile not found, run the following command to 	fix it:
    
    ```commandline
    automake --add-missing
    ```
    
    In case, if it complains about automake, run the following command to fix it:
    
    ```commandline
    automake --compile
    ```
    
    In case, you run make with wrong configurations, 'make clean' before rerunning the above script.
    
The oracc is now installed and ready for usage.

Finally, all your executable after the installation can be found at the Installation Directory specified above.
ox is the executable to check the atf files, which is under $InstallationDirectory/bin.
    
