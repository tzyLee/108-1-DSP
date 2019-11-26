FROM kaldiasr/kaldi:latest

RUN apt-get update && apt-get install -y libc6-dev-i386

COPY dsp-hw2-1 /opt/kaldi/dsp-hw2-1

RUN echo "source /opt/kaldi/dsp-hw2-1/0-activate.sh" >> ~/.bashrc
