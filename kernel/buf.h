struct buf {
  int valid;   // has data been read from disk?
  int disk;    // does disk "own" buf?
  uint dev;
  uint blockno;
  struct sleeplock lock;
  uint refcnt;
  // struct buf *prev; lab8: 删除这个双向链表的一个指针
  struct buf *next;
  uchar data[BSIZE];
   // lab8: 记录最后使用缓存块的时间
  uint timestamp;
};