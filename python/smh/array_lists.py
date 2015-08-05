# This file was automatically generated by SWIG (http://www.swig.org).
# Version 2.0.11
#
# Do not make changes to this file unless you know what you are doing--modify
# the SWIG interface file instead.





from sys import version_info
if version_info >= (2,6,0):
    def swig_import_helper():
        from os.path import dirname
        import imp
        fp = None
        try:
            fp, pathname, description = imp.find_module('_array_lists', [dirname(__file__)])
        except ImportError:
            import _array_lists
            return _array_lists
        if fp is not None:
            try:
                _mod = imp.load_module('_array_lists', fp, pathname, description)
            finally:
                fp.close()
            return _mod
    _array_lists = swig_import_helper()
    del swig_import_helper
else:
    import _array_lists
del version_info
try:
    _swig_property = property
except NameError:
    pass # Python < 2.2 doesn't have 'property'.
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'SwigPyObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError(name)

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

try:
    _object = object
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0


def _swig_setattr_nondynamic_method(set):
    def set_attr(self,name,value):
        if (name == "thisown"): return self.this.own(value)
        if hasattr(self,name) or (name == "this"):
            set(self,name,value)
        else:
            raise AttributeError("You cannot add attributes to %s" % self)
    return set_attr


class Item(object):
    thisown = _swig_property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    item = _swig_property(_array_lists.Item_item_get, _array_lists.Item_item_set)
    freq = _swig_property(_array_lists.Item_freq_get, _array_lists.Item_freq_set)
    def __init__(self): 
        this = _array_lists.new_Item()
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _array_lists.delete_Item
    __del__ = lambda self : None;
Item_swigregister = _array_lists.Item_swigregister
Item_swigregister(Item)

class List(object):
    thisown = _swig_property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    size = _swig_property(_array_lists.List_size_get, _array_lists.List_size_set)
    data = _swig_property(_array_lists.List_data_get, _array_lists.List_data_set)
    def __getitem__(self, *args): return _array_lists.List___getitem__(self, *args)
    def __init__(self): 
        this = _array_lists.new_List()
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _array_lists.delete_List
    __del__ = lambda self : None;
List_swigregister = _array_lists.List_swigregister
List_swigregister(List)


