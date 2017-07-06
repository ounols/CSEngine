#pragma once


//#define SINGLETONE(x) static inline x* getInstance() { static x t; return &t; }
// 복사 금지 매크로
#define MAKE_NO_COPY(CLASSNAME)                                             \
        private:                                                            \
               CLASSNAME(const CLASSNAME&);                                 \
               CLASSNAME& operator=(const CLASSNAME&);

// 싱클톤 패턴 생성 매크로
#define DECLARE_SINGLETONE(CLASSNAME)                                       \
        MAKE_NO_COPY(CLASSNAME)                                             \
        private:                                                            \
               CLASSNAME();                                               \
               static CLASSNAME* sInstance;                                 \
        public:                                                             \
               static CLASSNAME* getInstance();                                \
               static void delInstance();

// 싱글톤 패턴 구현 매크로
#define IMPLEMENT_SINGLETON(CLASSNAME)                              \
               CLASSNAME* CLASSNAME::sInstance= NULL;               \
                                                                    \
               CLASSNAME* CLASSNAME::getInstance() {                \
                       if(sInstance == NULL)                        \
                              sInstance = new CLASSNAME;              \
                              return sInstance;                    \
               }                                                    \
               void CLASSNAME::delInstance()                    \
               {                                                    \
                    SAFE_DELETE(sInstance);                        \
               }


#define NULLPTR(p) { (p) = nullptr; }
#define SAFE_DELETE(p) { if(p) delete (p); NULLPTR(p); }
#define SAFE_DELETE_SOBJECT(p) {if(p) MemoryMgr::getInstance()->ReleaseObject(p);}
#define SAFE_DELETE_SGAMEOBJECT(p) {if(p) GameObjectMgr::getInstance()->DeleteGameObject(p);}
#define SAFE_DELETE_ARRAY(p) { if(p) delete[] (p); NULLPTR(p); }
#define SAFE_DESTROY(p) { if(p) (p)->Destroy(); NULLPTR(p); }
#define SAFE_RELEASE(p) { if(p) (p)->Release(); }
