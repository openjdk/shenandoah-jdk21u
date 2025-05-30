/*
 * Copyright (c) 2022, Oracle and/or its affiliates. All rights reserved.
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
 */

/*
 * @test
 * @summary Testing Classfile TempConstantPoolBuilder.
 * @run junit TempConstantPoolBuilderTest
 */
import jdk.internal.classfile.*;
import jdk.internal.classfile.attribute.RuntimeVisibleAnnotationsAttribute;
import jdk.internal.classfile.attribute.SourceFileAttribute;
import java.lang.reflect.AccessFlag;
import org.junit.jupiter.api.Test;

import java.lang.constant.ClassDesc;

import static helpers.TestConstants.MTD_VOID;
import static java.lang.constant.ConstantDescs.CD_Object;
import static java.lang.constant.ConstantDescs.CD_void;
import java.lang.constant.MethodTypeDesc;
import static jdk.internal.classfile.Opcode.INVOKESPECIAL;
import static jdk.internal.classfile.TypeKind.VoidType;

class TempConstantPoolBuilderTest {

    public static final ClassDesc INTERFACE = ClassDesc.ofDescriptor("Ljava/lang/FunctionalInterface;");

    @Test
    void createAnno() {
        Annotation a = Annotation.of(INTERFACE,
                                     AnnotationElement.ofString("foo", "bar"));
    }

    @Test
    void addAnno() {
        byte[] bytes = Classfile.build(ClassDesc.of("MyClass"), cb -> {
            cb.withFlags(AccessFlag.PUBLIC)
              .with(SourceFileAttribute.of(cb.constantPool().utf8Entry(("MyClass.java"))))
              .withMethod("<init>", MethodTypeDesc.of(CD_void), 0, mb -> mb
                            .withCode(codeb -> codeb.loadInstruction(TypeKind.ReferenceType, 0)
                                    .invokeInstruction(INVOKESPECIAL, CD_Object, "<init>", MTD_VOID, false)
                                    .returnInstruction(VoidType)
                            )
                            .with(RuntimeVisibleAnnotationsAttribute.of(Annotation.of(INTERFACE,
                                                                                      AnnotationElement.ofString("foo", "bar"))))
              );
        });
        ClassModel m = Classfile.parse(bytes);
        //ClassPrinter.toJson(m, ClassPrinter.Verbosity.TRACE_ALL, System.out::println);
    }
}
