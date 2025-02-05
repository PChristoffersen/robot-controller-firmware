ARG ZEPHYR_BUILD_VERSION=v0.27.4
ARG ZEPHYR_BUILD_IMAGE=zephyrprojectrtos/zephyr-build
FROM ${ZEPHYR_BUILD_IMAGE}:${ZEPHYR_BUILD_VERSION} as base

USER root

# Base utils
RUN apt-get update && apt-get upgrade -y && \
    apt-get install -y \
        picocom \
        usbutils \
        psmisc \
        lsof \
        vim \
        less \
        udev \
        gdb-multiarch \
        dwarves \
    && apt-get clean

# Zephyr 
#RUN pip3 install git+https://github.com/HBehrens/puncover


# --------------------------------------------------------------
# Install newer version of OpenOCD
#

RUN apt-get install -y libusb-1.0-0 libhidapi-hidraw0 libhidapi-libusb0 && apt-get clean

FROM base AS openocd

ARG OPENOCD_VERSION=v0.12.0

RUN apt-get install -y libusb-1.0-0-dev libhidapi-dev

WORKDIR /workdir
RUN git clone --depth 1 --branch ${OPENOCD_VERSION} https://github.com/openocd-org/openocd.git  && cd openocd && ./bootstrap

RUN cd openocd && \
    ./configure --prefix /opt/toolchains/openocd && \
    make -j 8

RUN cd openocd && make install


# --------------------------------------------------------------
# Create runtime image
#

FROM base AS runtime

# Copy from other images
COPY --from=openocd /opt/toolchains/openocd /opt/toolchains/openocd

# Modify the user
RUN <<EOS
usermod --shell /bin/bash user
usermod -a -G dialout user
usermod -a -G plugdev user
usermod -a -G root user
EOS


# [Optional] Set the default user. Omit if you want to keep the default as root.
#USER user
