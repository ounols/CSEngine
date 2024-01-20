#pragma once


#define MAKE_NO_COPY(CLASSNAME)                                             \
        private:                                                            \
               CLASSNAME(const CLASSNAME&);                                 \
               CLASSNAME& operator=(const CLASSNAME&);


#define DECLARE_SINGLETON(CLASSNAME)                                       \
        MAKE_NO_COPY(CLASSNAME)                                             \
        protected:                                                            \
               explicit CLASSNAME();                                               \
               static CLASSNAME* sInstance;                                 \
        public:                                                             \
               static CLASSNAME* getInstance();                                \
               static void delInstance();


#define IMPLEMENT_SINGLETON(CLASSNAME)                              \
               CLASSNAME* CLASSNAME::sInstance= nullptr;               \
                                                                    \
               CLASSNAME* CLASSNAME::getInstance() {                \
                       if(sInstance == nullptr)                        \
                              sInstance = new CLASSNAME;              \
                              return sInstance;                    \
               }                                                    \
               void CLASSNAME::delInstance()                    \
               {                                                    \
                    SAFE_DELETE(sInstance);                        \
               }


#define NULLPTR(p) { (p) = nullptr; }
#define SAFE_DELETE(p) { delete (p); NULLPTR(p); }
#define SAFE_DELETE_SOBJECT(p) {if(p) MemoryMgr::getInstance()->ReleaseObject(p);}
#define SAFE_DELETE_SGAMEOBJECT(p) {if(p) GameObjectMgr::getInstance()->DeleteGameObject(p);}
#define SAFE_DELETE_ARRAY(p) { if(p) delete[] (p); NULLPTR(p); }
#define SAFE_DESTROY(p) { if(p) (p)->Destroy(); NULLPTR(p); }
#define SAFE_RELEASE(p) { if(p) (p)->Release(); }

