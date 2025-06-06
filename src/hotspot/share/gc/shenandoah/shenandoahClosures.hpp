/*
 * Copyright (c) 2019, 2022, Red Hat, Inc. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 *
 */
#ifndef SHARE_GC_SHENANDOAH_SHENANDOAHCLOSURES_HPP
#define SHARE_GC_SHENANDOAH_SHENANDOAHCLOSURES_HPP

#include "code/nmethod.hpp"
#include "memory/iterator.hpp"
#include "oops/accessDecorators.hpp"
#include "runtime/handshake.hpp"

class BarrierSetNMethod;
class ShenandoahBarrierSet;
class ShenandoahHeap;
class ShenandoahMarkingContext;
class ShenandoahHeapRegionSet;
class Thread;
class SATBMarkQueueSet;

class ShenandoahFlushSATBHandshakeClosure : public HandshakeClosure {
private:
  SATBMarkQueueSet& _qset;
public:
  inline explicit ShenandoahFlushSATBHandshakeClosure(SATBMarkQueueSet& qset);
  inline void do_thread(Thread* thread) override;
};

class ShenandoahForwardedIsAliveClosure: public BoolObjectClosure {
private:
  ShenandoahMarkingContext* const _mark_context;
public:
  inline ShenandoahForwardedIsAliveClosure();
  inline bool do_object_b(oop obj);
};

class ShenandoahIsAliveClosure: public BoolObjectClosure {
private:
  ShenandoahMarkingContext* const _mark_context;
public:
  inline ShenandoahIsAliveClosure();
  inline bool do_object_b(oop obj);
};

class ShenandoahIsAliveSelector : public StackObj {
private:
  ShenandoahIsAliveClosure _alive_cl;
  ShenandoahForwardedIsAliveClosure _fwd_alive_cl;
public:
  inline BoolObjectClosure* is_alive_closure();
};

class ShenandoahKeepAliveClosure : public OopClosure {
private:
  ShenandoahBarrierSet* const _bs;
public:
  inline ShenandoahKeepAliveClosure();
  inline void do_oop(oop* p);
  inline void do_oop(narrowOop* p);
private:
  template <typename T>
  void do_oop_work(T* p);
};

class ShenandoahOopClosureBase : public MetadataVisitingOopIterateClosure {
public:
  inline void do_nmethod(nmethod* nm);
};

class ShenandoahUpdateRefsClosure: public ShenandoahOopClosureBase {
private:
  ShenandoahHeap* _heap;
public:
  inline ShenandoahUpdateRefsClosure();
  inline void do_oop(oop* p);
  inline void do_oop(narrowOop* p);
private:
  template <class T>
  inline void do_oop_work(T* p);
};

template <bool concurrent, bool stable_thread>
class ShenandoahEvacuateUpdateRootClosureBase : public ShenandoahOopClosureBase {
protected:
  ShenandoahHeap* const _heap;
  Thread* const _thread;
public:
  inline ShenandoahEvacuateUpdateRootClosureBase();
  inline void do_oop(oop* p);
  inline void do_oop(narrowOop* p);
protected:
  template <class T>
  inline void do_oop_work(T* p);
};

using ShenandoahEvacuateUpdateMetadataClosure = ShenandoahEvacuateUpdateRootClosureBase<false, true>;
using ShenandoahEvacuateUpdateRootsClosure = ShenandoahEvacuateUpdateRootClosureBase<true, false>;
using ShenandoahContextEvacuateUpdateRootsClosure = ShenandoahEvacuateUpdateRootClosureBase<true, true>;

template <bool CONCURRENT, typename IsAlive, typename KeepAlive>
class ShenandoahCleanUpdateWeakOopsClosure : public OopClosure {
private:
  IsAlive*    _is_alive;
  KeepAlive*  _keep_alive;

public:
  inline ShenandoahCleanUpdateWeakOopsClosure(IsAlive* is_alive, KeepAlive* keep_alive);
  inline void do_oop(oop* p);
  inline void do_oop(narrowOop* p);
};

class ShenandoahCodeBlobAndDisarmClosure: public CodeBlobToOopClosure {
private:
  BarrierSetNMethod* const _bs;

public:
  inline ShenandoahCodeBlobAndDisarmClosure(OopClosure* cl);
  inline void do_code_blob(CodeBlob* cb);
};

#ifdef ASSERT
class ShenandoahAssertNotForwardedClosure : public OopClosure {
private:
  template <class T>
  inline void do_oop_work(T* p);

public:
  inline void do_oop(narrowOop* p);
  inline void do_oop(oop* p);
};
#endif // ASSERT

#endif // SHARE_GC_SHENANDOAH_SHENANDOAHCLOSURES_HPP
