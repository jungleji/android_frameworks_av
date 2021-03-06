/*
 * This file is part of libbluray
 * Copyright (C) 2012  Petri Hintukainen <phintuka@users.sourceforge.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#if !defined(_EXTDATA_PARSE_H_)
#define _EXTDATA_PARSE_H_

#include "attributes.h"
#include "bits.h"

#include <stdint.h>

int bdmv_parse_extension_data(BITSTREAM *bits,
                                         int start_address,
                                         int (*handler)(BITSTREAM*, int, int, void*),
                                         void *handle);

#endif // _EXTDATA_PARSE_H_
