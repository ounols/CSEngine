#pragma once



#define SINGLETONE(x) static inline x* getInstance() { static x t; return &t; }

#define NULLPTR(p) { (p) = nullptr; }
#define SAFE_DELETE(p) { if(p) delete (p); NULLPTR(p); }
#define SAFE_DELETE_ARRAY(p) { if(p) delete[] (p); NULLPTR(p); }
#define SAFE_DESTROY(p) { if(p) (p)->Destroy(); NULLPTR(p); }
#define SAFE_RELEASE(p) { if(p) (p)->Release(); }