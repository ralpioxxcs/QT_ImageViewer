# QT Imageviewer
simple imageviewer using qt5 framework

## How to run
```Bash
docker build -t qt_imageviewer .

xhost + 127.0.0.1
docker run  -it --rm \
            -e DISPLAY=host.docker.internal:0
            qt_imageviewer:latest
```
        


