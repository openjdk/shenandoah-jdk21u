/*
 * Copyright (c) 1999, 2023, Oracle and/or its affiliates. All rights reserved.
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

#include "precompiled.hpp"
#include "ci/ciUtilities.hpp"
#include "gc/shared/cardTableBarrierSet.hpp"
#include "gc/shared/cardTable.hpp"
#include "gc/shared/collectedHeap.hpp"
#include "gc/shared/gc_globals.hpp"

// ciUtilities
//
// Miscellaneous internal compiler interface routines.

// ------------------------------------------------------------------
// basictype_to_str
const char* basictype_to_str(BasicType t) {
  const char* str = type2name(t);
  if (str == nullptr) return "illegal";
  return str;
}

// ------------------------------------------------------------------
// card_table_base
CardTable::CardValue* ci_card_table_address() {
  BarrierSet* bs = BarrierSet::barrier_set();
  CardTableBarrierSet* ctbs = barrier_set_cast<CardTableBarrierSet>(bs);
  CardTable* ct = ctbs->card_table();
  assert(!UseShenandoahGC, "Shenandoah byte_map_base is not constant.");
  return ct->byte_map_base();
}
