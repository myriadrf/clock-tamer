/*
        ClockTamer - a software for configurable reference clock
                  Copyright (C) 2009, Fairwaves
          by Sergey Kostanbaev <Sergey.Kostanbaev@fairwaves.ru>
*/

/*
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef LMK0X0XX
#define LMK0X0XX

#define MAKE_LMK(mux, en, div, dly, r) \
    (((((((((int24_t)mux << 1) | en) << 8) | div ) << 4) | dly ) << 4) | r)

#define MAKE_LMK_LL(dly, r)     (((dly) << 4) | r)
#define MAKE_LMK_LH(div)        (div)
#define MAKE_LMK_HL(mux, en)    (((mux) << 1) | en)
#define MAKE_LMK_HH()           0

#endif