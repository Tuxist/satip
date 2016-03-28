class MediaBackend {
public:
  MediaBackend();
  ~MediaBackend();
  
  void mute();
  void unmute();
  void louder();
  void lower();
  
  void play(void *data,size_t size);
  void pause();
  void stop();
private:
  
};
