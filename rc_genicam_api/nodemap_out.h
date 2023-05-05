/*
 * This file is part of the rc_genicam_api package.
 *
 * Copyright (c) 2023 Roboception GmbH
 * All rights reserved
 *
 * Author: Heiko Hirschmueller
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef NODEMAP_OUT_H
#define NODEMAP_OUT_H

#include <rc_genicam_api/config.h>

namespace rcg
{

/**
  Takes an integer value and formats it according to the specification in the
  node.

  @param node  Node.
  @param value Integer value.
  @return      Formated value.
*/

std::string formatValue(GenApi::IInteger *node, int64_t value);

/**
  Recursive printing of nodes to standard out.

  @param prefix Prefix that will be prepended to every line.
  @param node   Node to be printed.
  @param depth  This value is reduced when calling printNode() recursively on
                category nodes. If the value is <= 0, then no recursion is
                done.
  @param show_enum_list If true, all possible enum values of a parameter are
                printed as well.
*/

void printNode(const std::string &prefix, GenApi::INode *node, int depth, bool show_enum_list);

/**
  Printing of nodemap, starting at given root node.

  @param nodemap Nodemap to be printed.
  @param root    Root node that will be printed with all child nodes.
  @param depth   Maximum depth that will be printed.
  @param show_enum_list If true, all possible enum values of a parameter are
                 printed as well.
  @return        False if root node cannot be found.
*/

bool printNodemap(const std::shared_ptr<GenApi::CNodeMapRef> &nodemap, const char root[],
  int depth=100, bool show_enum_list=true);

}

#endif
