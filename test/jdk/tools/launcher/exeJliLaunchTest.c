/*
 * Copyright (c) 2018, 2024, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.  Oracle designates this
 * particular file as subject to the "Classpath" exception as provided
 * by Oracle in the LICENSE file that accompanied this code.
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
 * This file contains the main entry point into the launcher code
 * this is the only file which will be repeatedly compiled by other
 * tools. The rest of the files will be linked in.
 */

#include "export.h"
#include "java.h"

EXPORT int
main(int argc, char **args)
{
    //avoid null-terminated array of arguments to test JDK-8303669
    char **argv = malloc(sizeof(char *) * argc);
    for (int i = 0; i < argc; i++) {
        argv[i] = args[i];
    }

    int ret = JLI_Launch(argc, argv,
                   0, NULL,
                   0, NULL,
                   "1", "0",
                   *argv, *argv,
                   0, 0, 0, 0);
    free(argv);
    return ret;
}
