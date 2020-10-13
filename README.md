# QT Imageviewer
simple imageviewer using qt5 framework

## How to run
### local
```Bash
cmake -H. -BRelease
cd Release
cmake --build .
```

### docker
you can build docker image  
```Bash
docker build -t qt_imageviewer .
```
build is done, run container configuring x11
```Bash
docker run  --rm --name qt_imageviewer \
            --volume /tmp/.X11-unix:/tmp/.X11-unix:ro \
            --volume /home/[username]/.Xauthority:/root/.Xauthority \
            -e USER=$USER \
            -e DISPLAY=$DISPLAY \
            --net host \
            qt_imageviewer
```
if using OSX
```Bash
xhost + 127.0.0.1
docker run  --rm --name qt_imageviewer \
            -e DISPLAY=host.docker.internal:0
            qt_imageviewer:latest
```
        


