FROM centos:7
LABEL org.label-schema.schema-version=1.0 org.label-schema.name="CentOS Base Image" org.label-schema.vendor="CentOS" org.label-schema.license="GPLv2" org.label-schema.build-date=20180531
RUN yum update -y  \
	&& yum groupinstall -y "Development Tools"
RUN yum install -y wget cmake tbb-devel gmp-devel mpfr-devel libmpc-devel
RUN wget -O /tmp/gcc-5.4.0.tar.bz2 https://ftp.gnu.org/gnu/gcc/gcc-5.4.0/gcc-5.4.0.tar.bz2
WORKDIR /tmp
RUN tar xfj gcc-5.4.0.tar.bz2
WORKDIR /tmp/gcc-5.4.0
RUN ./configure --enable-languages=c,c++ --disable-multilib
RUN make -j  \
	&& make install
RUN rm -r /tmp/gcc-5.4.0*
WORKDIR /tmp
RUN wget https://downloads.sourceforge.net/project/boost/boost/1.55.0/boost_1_55_0.tar.bz2
RUN tar -xf boost_1_55_0.tar.bz2
WORKDIR /tmp/boost_1_55_0
RUN ./bootstrap.sh --with-libraries=coroutine,filesystem,program_options  \
	&& ./b2 install
WORKDIR /
RUN rm -r /tmp/boost_1_55_0*
RUN wget -O /tmp/capnproto.tar.gz https://github.com/sandstorm-io/capnproto/archive/v0.4.0.tar.gz
WORKDIR /tmp
RUN tar -xf /tmp/capnproto.tar.gz
WORKDIR /tmp/capnproto-0.4.0/c++
RUN autoreconf -i
RUN CXXFLAGS=-fpermissive ./configure
RUN make -j  \
	&& make install
WORKDIR /
RUN rm -r /tmp/capnproto*
RUN yum groupremove -y "Development Tools"
RUN yum install -y git make
RUN yum remove -y gmp-devel mpfr-devel libmpc-devel
RUN yum clean -y all
WORKDIR /tmp
RUN git clone https://www.github.com/SESA/EbbRT
WORKDIR /tmp/EbbRT
RUN make -j hosted  \
	&& make hosted-install
WORKDIR /
RUN rm -r /tmp/EbbRT