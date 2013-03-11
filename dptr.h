#ifndef DPTR_H
#define DPTR_H

#define DPTR_INIT(p) dptr(p)
//put in protected
#define DPTR_DECLARE(Class) DPtrInterface<Class, Class##Private> dptr;
//put in private
#define DPTR_DECLARE_PRIVATE(Class) \
    inline Class##Private& d_func() { return dptr.pri<Class##Private>(); } \
    inline const Class##Private& d_func() const { return dptr.pri<Class##Private>(); } \
    friend class Class##Private;

#define DPTR_DECLARE_PUBLIC(Class) \
    inline Class& q_func() { return *static_cast<Class*>(dptr_ptr()); } \
    inline const Class& q_func() const { return *static_cast<const Class*>(dptr_ptr()); } \
    friend class Class;

#define DPTR_INIT_PRIVATE(Class) dptr.setPublic(this);
#define DPTR_D(Class) Class##Private& d = dptr.pri<Class##Private>()
#define DPTR_P(Class) Class& p = *static_cast<Class*>(dptr_ptr())

//interface
template <typename PUB>
class DPtrPrivate
{
public:
    virtual ~DPtrPrivate() {}
    inline void DPTR_setPublic(PUB* pub) { dptr_p_ptr = pub; }
protected:
    inline PUB& dptr_p() { return *dptr_p_ptr; }
    inline const PUB& dptr_p() const { return *dptr_p_ptr; }
    inline PUB* dptr_ptr() { return dptr_p_ptr; }
    inline const PUB* dptr_ptr() const { return dptr_p_ptr; }
private:
    PUB* dptr_p_ptr;
};

//interface
template <typename PUB, typename PVT>
class DPtrInterface
{
    friend class DPtrPrivate<PUB>;
public:
    DPtrInterface(PVT* d):pvt(d) {}
    DPtrInterface():pvt(new PVT) {}
    ~DPtrInterface() {
        if (pvt) {
            delete pvt;
            pvt = 0;
        }
    }
    inline void setPublic(PUB* pub) { pvt->DPTR_setPublic(pub); }
    template <typename T>
    inline T& pri() { return *reinterpret_cast<T*>(pvt); }
    template <typename T>
    inline const T& pri() const { return *reinterpret_cast<T*>(pvt); } //static cast requires defination of T
    inline PVT& operator()() { return *static_cast<PVT*>(pvt); }
    inline const PVT& operator()() const { return *static_cast<PVT*>(pvt); }
    inline PVT * operator->() { return static_cast<PVT*>(pvt); }
    inline const PVT * operator->() const { return static_cast<PVT*>(pvt); }
private:
    DPtrInterface(const DPtrInterface&);
    DPtrInterface& operator=(const DPtrInterface&);
    DPtrPrivate<PUB>* pvt;
};

//不属于DPTR, 但是又常用的东西, 暂时放这里

//暂时, 应该加入一个判断, 判断是否支持C++11特性
#define DISABLE_COPY(Class) \
    Class(const Class &); \
    Class &operator = (const Class &);

#endif // DPTR_H
