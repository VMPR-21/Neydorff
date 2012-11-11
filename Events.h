#ifndef _EVENTS_H
#define _EVENTS_H

//General C++ event handling support. C++11 lamdas could be used too.
//by chgena@mail.ru

#include <vector>
#include <algorithm>
#include <set>

#define AUTO_REMOVE_SUPPORT

#ifdef QT_NO_DYNAMIC_CAST
#define dynamic_cast reinterpret_cast//if NO rtti available bad HACK!!!!!!!!!!!
#endif

#define EVENT(ev_name,arg_type) private: \
    EventsMap<arg_type> ev_name##Event;\
    public: HandlersMan<arg_type> ev_name;

#define STATIC_EVENT(ev_name,arg_type) private: \
    static EventsMap<arg_type> ev_name##Event;\
    public: static HandlersMan<arg_type> ev_name;

namespace _Type_Checks//support code for type_cast<>
{
//exactly dsame types
template <typename T, typename U>
struct same_type
{
    static const bool value = false;
};
template <typename T>
struct same_type< T, T >
{
    static const bool value = true;
};
//supports public inheritance
template<class Base, class Derived>
struct SubclassCheck
{
    typedef char Yes;
    class No
    {
        char dummy[2];
    };

    static Yes Test(Base*);
    static No Test(...);
    enum
    {
        Value = (sizeof(Test((Derived*)NULL)) == sizeof(Yes))
    };
};
}

template<typename TReq, typename TArg> TReq* type_cast(TArg *arg) //checks if arg the type that required
{
    if(_Type_Checks::SubclassCheck<TReq, TArg>::Value)
        return (TReq*)arg;

    return NULL;
}

template<typename T> class HandlersMan;
class IHandlersAutoRemove;


//special classes for ret vals void support
template<typename Targ, typename Tret> class EventBaseHelper
{
public:
    virtual Tret operator()(const Targ &arg) = 0;
    Tret fire(const Targ &arg)
    {
        return this->operator ()(arg);
    }

    virtual ~EventBaseHelper() {}
};
template<typename Targ> class EventBaseHelper<Targ, void> //void specialization
{
public:
    virtual void operator()(const Targ &arg) = 0;
    void fire(const Targ &arg)
    {
        this->operator ()(arg);   //void ret. val.
    }

    virtual ~EventBaseHelper() {}
};

//base and abstract event class
template<typename Targ, typename Tret = void>
class Event: public EventBaseHelper<Targ, Tret>
{
public:
    virtual ~Event(void) {}
    Event() {}
    Event(const Event&) {}
    virtual bool operator ==(Event<Targ, Tret> &b) = 0;

    virtual IHandlersAutoRemove* obj() const = 0;
};

//Event args base class stub
class EventArgs
{
public:
    virtual ~EventArgs() {}
};

//special classes for void ret. val. support
template<typename Targ, typename Tobj, typename TRArg, typename Tret>
class EventHandlerBaseHelper: public Event<Targ, Tret>
{
public:
    Tret operator ()(const Targ &arg)
    {
        return (oo->*hh)(TRArg(arg));
    }
    typedef Tret (Tobj::*Handler)(const TRArg&);
protected:
    Tobj *oo;
    Handler hh;
};
template<typename Targ, typename Tobj, typename TRArg>
class EventHandlerBaseHelper<Targ, Tobj, TRArg, void>: public Event<Targ, void>
{

public:
    void operator ()(const Targ &arg)
    {
        (oo->*hh)(TRArg(arg));
    }
    typedef void (Tobj::*Handler)(const TRArg&);
protected:
    Tobj *oo;
    Handler hh;
};

//event handler for any member of class: "void <Class>::member(T &arg)"
template<typename Targ, typename Tobj, typename TRArg = Targ, typename Tret = void>
class EventHandler: public EventHandlerBaseHelper<Targ, Tobj, TRArg, Tret>
{
    typedef Tret (Tobj::*Handler)(const TRArg&);

public:
    EventHandler(Tobj *o, Handler h)
    {
        EventHandlerBaseHelper<Targ, Tobj, TRArg, Tret>::oo = o;
        EventHandlerBaseHelper<Targ, Tobj, TRArg, Tret>::hh = h;
    }

    bool operator ==(Event<Targ, Tret> &b)
    {
        EventHandler<Targ, Tobj, TRArg, Tret> *e = dynamic_cast<EventHandler<Targ, Tobj, TRArg, Tret>*>(&b);

        if(e && e->hh == EventHandlerBaseHelper<Targ, Tobj, TRArg, Tret>::hh &&
                e->oo == EventHandlerBaseHelper<Targ, Tobj, TRArg, Tret>::oo)
            return true;

        return false;
    }

    IHandlersAutoRemove* obj() const
    {
        return type_cast<IHandlersAutoRemove>(EventHandlerBaseHelper<Targ, Tobj, TRArg, Tret>::oo);
    }
};

//helper classes for void ret. val.
template<typename Targ, typename Tobj, typename TRArg, typename Tret>
class EventHandlerFBaseHelper: public Event<Targ, Tret>
{
protected:
    Tobj o;
public:
    EventHandlerFBaseHelper(Tobj o): o(o)
    {
    }
    Tret operator ()(const Targ &arg)
    {
        return o(TRArg(arg));
    }
};
template<typename Targ, typename Tobj, typename TRArg>
class EventHandlerFBaseHelper<Targ, Tobj, TRArg, void>: public Event<Targ, void>
{
protected:
    Tobj o;
public:
    EventHandlerFBaseHelper(Tobj o): o(o)
    {
    }
    void operator ()(const Targ &arg)
    {
        o(TRArg(arg));
    }
};

template<typename Targ, class Tobj, typename TRArg = Targ, typename Tret = void>
class EventHandlerF: public EventHandlerFBaseHelper<Targ, Tobj, TRArg, Tret>
{

public:
    EventHandlerF(Tobj o): EventHandlerFBaseHelper<Targ, Tobj, TRArg, Tret>(o)
    {

    }
    bool operator ==(Event<Targ, Tret> &b)
    {
        EventHandlerF<Targ, Tobj, TRArg, Tret> *e = dynamic_cast<EventHandlerF<Targ, Tobj, TRArg, Tret>*>(&b);

        if(e)
            return true;

        // return e->o==EventHandlerFBaseHelper<Targ,Tobj,TRArg,Tret>::o; <--not for lambdas
        return false;
    }

    IHandlersAutoRemove* obj() const
    {
        return NULL;
    }
};

template<typename T> class EventsMap
{
    static EventsMap<T> *lastCreated;
protected:
    std::vector<Event<T>*> handlers;
    friend class HandlersMan<T>;
public:
    EventsMap()
    {
        EventsMap<T>::lastCreated = this;
    }
    virtual ~EventsMap()
    {
        //clear events
        for(size_t i = 0; i < handlers.size(); i++)
        {
            delete handlers[i];
        }
    }
    virtual void operator()(const T &arg)
    {
        for(size_t i = 0; i < handlers.size(); i++)
            handlers[i]->operator()(arg);
    }
    void fire(const T &arg)
    {
        this->operator()(arg);
    }
};

template<typename T>
EventsMap<T>* EventsMap<T>::lastCreated = NULL;

class HandlersManBase;

class IHandlersAutoRemove
{
public:
    virtual void reg(HandlersManBase *hm) = 0;
    virtual void unreg(HandlersManBase *hm) = 0;
};

class HandlersManBase
{
protected:

    void regObj(void* /* obj */)
    {
    }
    void regObj(IHandlersAutoRemove *obj)
    {
        IHandlersAutoRemove *r = static_cast<IHandlersAutoRemove*>(obj);

        if(!r)
            return;

        r->reg(this);
    }

    void unregObj(void* /* obj */)
    {
    }
    void unregObj(IHandlersAutoRemove *obj)
    {
        IHandlersAutoRemove *r = dynamic_cast<IHandlersAutoRemove*>(obj);

        if(!r)
            return;

        r->unreg(this);
    }
public:
    //removes all handlers for obj
    virtual void del(void *obj) = 0;

    virtual ~HandlersManBase() {}
};

//base class. provides auto removing handlers from events when object destroys
class HandlersAutoRemove: public IHandlersAutoRemove
{
    std::set<HandlersManBase*> _handlers;
    typedef std::set<HandlersManBase*>::const_iterator hc_iter;
public:
    HandlersAutoRemove()
    {
    }
    void reg(HandlersManBase *hm)
    {
        _handlers.insert(hm);
    }
    void unreg(HandlersManBase *hm)
    {
        _handlers.erase(hm);
    }
    virtual ~HandlersAutoRemove()
    {
        if(!_handlers.size())
            return;

        for(hc_iter it = _handlers.begin(); it != _handlers.end(); ++it)
        {
            (*it)->del(this);
        }

        _handlers.clear();
    }
};

template<typename T> class HandlersMan: public HandlersManBase
{
    EventsMap<T> *map;

    //STL functors
    template<typename R> struct ptr_eq
    {
        R *a;
        ptr_eq(R *a): a(a) {}
        bool operator()(R *b)
        {
            return *a == *b;
        }
    };
    template<typename R> struct event_obj_equ
    {
        void *_obj;
        event_obj_equ(void *obj): _obj(obj) {}
        bool operator()(Event<R> *e)
        {
            return e->obj() == _obj;
        }
    };
    template<typename R> struct unreg_f
    {
        HandlersMan<R> *hm;
        unreg_f(HandlersMan<R> *hm): hm(hm) {}
        void operator()(Event<R> *e)
        {
            IHandlersAutoRemove *h = e->obj();

            if(!h)
                return;

            hm->unregObj(h);
        }
    };
public:
    HandlersMan()
    {
        map = EventsMap<T>::lastCreated;
    }
    ~HandlersMan()
    {
        std::for_each(map->handlers.begin(), map->handlers.end(), unreg_f<T>(this));
        map->handlers.clear();
    }
    HandlersMan& operator+=(Event<T>* event)
    {
        map->handlers.push_back(event);
        return *this;
    }
    HandlersMan& operator-=(Event<T>* event)
    {
        map->handlers.erase(std::remove_if(
                                map->handlers.begin(), map->handlers.end(), ptr_eq<T>(event)), map->handlers.end());
        delete event;
        return *this;
    }
    //functions for easy events addition
    template<typename Tobj, typename Targ>
    void add(Tobj *obj, void (Tobj::*handler)(const Targ&) )
    {
        operator+=(new EventHandler<Targ, Tobj>(obj, handler));
        regObj(obj);
    }
    template<typename Tobj, typename Targ>
    void del(Tobj *obj, void (Tobj::*handler)(const Targ&) )
    {
        operator-=(new EventHandler<Targ, Tobj>(obj, handler));
        unregObj(obj);
    }

    template<typename Tobj>
    void add(Tobj obj)
    {
        operator+=(new EventHandlerF<T, Tobj, T, void>(obj));
    }
    template<typename Tobj>
    void del(Tobj obj)
    {
        operator-=(new EventHandlerF<T, Tobj, T, void>(obj));
    }

    //removes all handlers for obj
    void del(void *obj)
    {
        map->handlers.erase(std::remove_if(
                                map->handlers.begin(), map->handlers.end(), event_obj_equ<T>(obj)), map->handlers.end());
    }

};

//functions for easy events creating

template<typename Targ, typename Tret> //functions,functors & lamdas
Event<Targ, Tret>* _evh(Tret (*fn)(const Targ&) )
{
    return new EventHandlerF<Targ, Tret (*)(const Targ&), Targ, Tret>(fn);
}

template<typename Targ, typename Tobj, typename Tret> //TRArg will be inferreed(TRArg==Targ)
Event<Targ, Tret>* _evh(Tobj *obj, Tret (Tobj::*handler)(const Targ&) )
{
    return new EventHandler<Targ, Tobj, Targ, Tret>(obj, handler);
}

template <typename Targ, typename TRArg, typename Tobj, typename Tret> //TRArg explicitly specified
Event<Targ, Tret>* _evh(Tobj *obj, Tret (Tobj::*handler)(const TRArg&) )
{
    return new EventHandler<Targ, Tobj, TRArg>(obj, handler);
}

#endif
