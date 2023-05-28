/* verilator lint_off WIDTH */
module IFU(
  input  [31:0] io_IFU_I_inst
);
endmodule
module IDU(
  input  [31:0] io_IDU_I_inst,
  output [4:0]  io_IDU_O_rs1,
  output [4:0]  io_IDU_O_rs2,
  output [4:0]  io_IDU_O_rd,
  output [5:0]  io_IDU_O_EXUopcode,
  output [5:0]  io_IDU_O_LSUopcode,
  output        io_IDU_O_GPRneedWriteBack,
  output [63:0] io_IDU_O_imm,
  output        io_IDU_O_halt
);
  wire [11:0] immI = io_IDU_I_inst[31:20]; // @[npc.scala 222:29]
  wire [51:0] _SignExtend_immI_T_2 = immI[11] ? 52'hfffffffffffff : 52'h0; // @[Bitwise.scala 77:12]
  wire [63:0] SignExtend_immI = {_SignExtend_immI_T_2,immI}; // @[Cat.scala 33:92]
  wire [11:0] immS = {io_IDU_I_inst[31:25],io_IDU_I_inst[11:7]}; // @[Cat.scala 33:92]
  wire [51:0] _SignExtend_immS_T_2 = immS[11] ? 52'hfffffffffffff : 52'h0; // @[Bitwise.scala 77:12]
  wire [63:0] SignExtend_immS = {_SignExtend_immS_T_2,io_IDU_I_inst[31:25],io_IDU_I_inst[11:7]}; // @[Cat.scala 33:92]
  wire [12:0] immB = {io_IDU_I_inst[31],io_IDU_I_inst[7],io_IDU_I_inst[30:25],io_IDU_I_inst[11:8],1'h0}; // @[Cat.scala 33:92]
  wire [50:0] _SignExtend_immB_T_2 = immB[12] ? 51'h7ffffffffffff : 51'h0; // @[Bitwise.scala 77:12]
  wire [63:0] SignExtend_immB = {_SignExtend_immB_T_2,io_IDU_I_inst[31],io_IDU_I_inst[7],io_IDU_I_inst[30:25],
    io_IDU_I_inst[11:8],1'h0}; // @[Cat.scala 33:92]
  wire [31:0] immU = {io_IDU_I_inst[31:12],12'h0}; // @[Cat.scala 33:92]
  wire [31:0] _SignExtend_immU_T_2 = immU[31] ? 32'hffffffff : 32'h0; // @[Bitwise.scala 77:12]
  wire [63:0] SignExtend_immU = {_SignExtend_immU_T_2,io_IDU_I_inst[31:12],12'h0}; // @[Cat.scala 33:92]
  wire [20:0] immJ = {io_IDU_I_inst[31],io_IDU_I_inst[19:12],io_IDU_I_inst[20],io_IDU_I_inst[30:21],1'h0}; // @[Cat.scala 33:92]
  wire [42:0] _SignExtend_immJ_T_2 = immJ[20] ? 43'h7ffffffffff : 43'h0; // @[Bitwise.scala 77:12]
  wire [63:0] SignExtend_immJ = {_SignExtend_immJ_T_2,io_IDU_I_inst[31],io_IDU_I_inst[19:12],io_IDU_I_inst[20],
    io_IDU_I_inst[30:21],1'h0}; // @[Cat.scala 33:92]
  wire [83:0] SignExtend_immR = {63'h0,io_IDU_I_inst[31],io_IDU_I_inst[19:12],io_IDU_I_inst[20],io_IDU_I_inst[30:21],1'h0
    }; // @[Cat.scala 33:92]
  wire [31:0] _IDU_opcodes_T = io_IDU_I_inst & 32'h7f; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_1 = 32'h37 == _IDU_opcodes_T; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_3 = 32'h17 == _IDU_opcodes_T; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_5 = 32'h6f == _IDU_opcodes_T; // @[Lookup.scala 31:38]
  wire [31:0] _IDU_opcodes_T_6 = io_IDU_I_inst & 32'h707f; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_7 = 32'h67 == _IDU_opcodes_T_6; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_9 = 32'h63 == _IDU_opcodes_T_6; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_11 = 32'h1063 == _IDU_opcodes_T_6; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_13 = 32'h4063 == _IDU_opcodes_T_6; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_15 = 32'h5063 == _IDU_opcodes_T_6; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_17 = 32'h6063 == _IDU_opcodes_T_6; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_19 = 32'h7063 == _IDU_opcodes_T_6; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_21 = 32'h3 == _IDU_opcodes_T_6; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_23 = 32'h1003 == _IDU_opcodes_T_6; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_25 = 32'h2003 == _IDU_opcodes_T_6; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_27 = 32'h4003 == _IDU_opcodes_T_6; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_29 = 32'h5003 == _IDU_opcodes_T_6; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_31 = 32'h23 == _IDU_opcodes_T_6; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_33 = 32'h1023 == _IDU_opcodes_T_6; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_35 = 32'h2023 == _IDU_opcodes_T_6; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_37 = 32'h13 == _IDU_opcodes_T_6; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_39 = 32'h2013 == _IDU_opcodes_T_6; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_41 = 32'h3013 == _IDU_opcodes_T_6; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_43 = 32'h4013 == _IDU_opcodes_T_6; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_45 = 32'h6013 == _IDU_opcodes_T_6; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_47 = 32'h7013 == _IDU_opcodes_T_6; // @[Lookup.scala 31:38]
  wire [31:0] _IDU_opcodes_T_48 = io_IDU_I_inst & 32'hfc00707f; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_49 = 32'h1013 == _IDU_opcodes_T_48; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_51 = 32'h5013 == _IDU_opcodes_T_48; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_53 = 32'h40005013 == _IDU_opcodes_T_48; // @[Lookup.scala 31:38]
  wire [31:0] _IDU_opcodes_T_54 = io_IDU_I_inst & 32'hfe00707f; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_55 = 32'h33 == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_57 = 32'h40000033 == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_59 = 32'h1033 == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_61 = 32'h2033 == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_63 = 32'h3033 == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_65 = 32'h4033 == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_67 = 32'h5033 == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_69 = 32'h40005033 == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_71 = 32'h6033 == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_73 = 32'h7033 == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_75 = 32'h100073 == io_IDU_I_inst; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_77 = 32'h6003 == _IDU_opcodes_T_6; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_79 = 32'h3003 == _IDU_opcodes_T_6; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_81 = 32'h3023 == _IDU_opcodes_T_6; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_83 = 32'h1b == _IDU_opcodes_T_6; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_85 = 32'h101b == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_87 = 32'h501b == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_89 = 32'h4000501b == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_91 = 32'h3b == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_95 = 32'h4000003b == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_97 = 32'h103b == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_99 = 32'h503b == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_101 = 32'h4000503b == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_103 = 32'h2000033 == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_105 = 32'h2001033 == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_107 = 32'h2002033 == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_109 = 32'h2003033 == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_111 = 32'h2004033 == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_113 = 32'h2005033 == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_115 = 32'h2006033 == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_117 = 32'h2007033 == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_119 = 32'h200003b == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_121 = 32'h200403b == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_123 = 32'h200503b == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_125 = 32'h200603b == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_127 = 32'h200703b == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire [2:0] _IDU_opcodes_T_128 = _IDU_opcodes_T_127 ? 3'h0 : 3'h7; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_129 = _IDU_opcodes_T_125 ? 3'h0 : _IDU_opcodes_T_128; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_130 = _IDU_opcodes_T_123 ? 3'h0 : _IDU_opcodes_T_129; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_131 = _IDU_opcodes_T_121 ? 3'h0 : _IDU_opcodes_T_130; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_132 = _IDU_opcodes_T_119 ? 3'h0 : _IDU_opcodes_T_131; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_133 = _IDU_opcodes_T_117 ? 3'h0 : _IDU_opcodes_T_132; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_134 = _IDU_opcodes_T_115 ? 3'h0 : _IDU_opcodes_T_133; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_135 = _IDU_opcodes_T_113 ? 3'h0 : _IDU_opcodes_T_134; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_136 = _IDU_opcodes_T_111 ? 3'h0 : _IDU_opcodes_T_135; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_137 = _IDU_opcodes_T_109 ? 3'h0 : _IDU_opcodes_T_136; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_138 = _IDU_opcodes_T_107 ? 3'h0 : _IDU_opcodes_T_137; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_139 = _IDU_opcodes_T_105 ? 3'h0 : _IDU_opcodes_T_138; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_140 = _IDU_opcodes_T_103 ? 3'h0 : _IDU_opcodes_T_139; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_141 = _IDU_opcodes_T_101 ? 3'h0 : _IDU_opcodes_T_140; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_142 = _IDU_opcodes_T_99 ? 3'h0 : _IDU_opcodes_T_141; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_143 = _IDU_opcodes_T_97 ? 3'h0 : _IDU_opcodes_T_142; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_144 = _IDU_opcodes_T_95 ? 3'h0 : _IDU_opcodes_T_143; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_145 = _IDU_opcodes_T_83 ? 3'h0 : _IDU_opcodes_T_144; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_146 = _IDU_opcodes_T_91 ? 3'h0 : _IDU_opcodes_T_145; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_147 = _IDU_opcodes_T_89 ? 3'h1 : _IDU_opcodes_T_146; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_148 = _IDU_opcodes_T_87 ? 3'h1 : _IDU_opcodes_T_147; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_149 = _IDU_opcodes_T_85 ? 3'h1 : _IDU_opcodes_T_148; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_150 = _IDU_opcodes_T_83 ? 3'h1 : _IDU_opcodes_T_149; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_151 = _IDU_opcodes_T_81 ? 3'h2 : _IDU_opcodes_T_150; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_152 = _IDU_opcodes_T_79 ? 3'h1 : _IDU_opcodes_T_151; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_153 = _IDU_opcodes_T_77 ? 3'h1 : _IDU_opcodes_T_152; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_154 = _IDU_opcodes_T_75 ? 3'h6 : _IDU_opcodes_T_153; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_155 = _IDU_opcodes_T_73 ? 3'h0 : _IDU_opcodes_T_154; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_156 = _IDU_opcodes_T_71 ? 3'h0 : _IDU_opcodes_T_155; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_157 = _IDU_opcodes_T_69 ? 3'h0 : _IDU_opcodes_T_156; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_158 = _IDU_opcodes_T_67 ? 3'h0 : _IDU_opcodes_T_157; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_159 = _IDU_opcodes_T_65 ? 3'h0 : _IDU_opcodes_T_158; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_160 = _IDU_opcodes_T_63 ? 3'h0 : _IDU_opcodes_T_159; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_161 = _IDU_opcodes_T_61 ? 3'h0 : _IDU_opcodes_T_160; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_162 = _IDU_opcodes_T_59 ? 3'h0 : _IDU_opcodes_T_161; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_163 = _IDU_opcodes_T_57 ? 3'h0 : _IDU_opcodes_T_162; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_164 = _IDU_opcodes_T_55 ? 3'h0 : _IDU_opcodes_T_163; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_165 = _IDU_opcodes_T_53 ? 3'h1 : _IDU_opcodes_T_164; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_166 = _IDU_opcodes_T_51 ? 3'h1 : _IDU_opcodes_T_165; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_167 = _IDU_opcodes_T_49 ? 3'h1 : _IDU_opcodes_T_166; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_168 = _IDU_opcodes_T_47 ? 3'h1 : _IDU_opcodes_T_167; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_169 = _IDU_opcodes_T_45 ? 3'h1 : _IDU_opcodes_T_168; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_170 = _IDU_opcodes_T_43 ? 3'h1 : _IDU_opcodes_T_169; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_171 = _IDU_opcodes_T_41 ? 3'h1 : _IDU_opcodes_T_170; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_172 = _IDU_opcodes_T_39 ? 3'h1 : _IDU_opcodes_T_171; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_173 = _IDU_opcodes_T_37 ? 3'h1 : _IDU_opcodes_T_172; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_174 = _IDU_opcodes_T_35 ? 3'h2 : _IDU_opcodes_T_173; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_175 = _IDU_opcodes_T_33 ? 3'h2 : _IDU_opcodes_T_174; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_176 = _IDU_opcodes_T_31 ? 3'h2 : _IDU_opcodes_T_175; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_177 = _IDU_opcodes_T_29 ? 3'h1 : _IDU_opcodes_T_176; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_178 = _IDU_opcodes_T_27 ? 3'h1 : _IDU_opcodes_T_177; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_179 = _IDU_opcodes_T_25 ? 3'h1 : _IDU_opcodes_T_178; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_180 = _IDU_opcodes_T_23 ? 3'h1 : _IDU_opcodes_T_179; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_181 = _IDU_opcodes_T_21 ? 3'h1 : _IDU_opcodes_T_180; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_182 = _IDU_opcodes_T_19 ? 3'h3 : _IDU_opcodes_T_181; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_183 = _IDU_opcodes_T_17 ? 3'h3 : _IDU_opcodes_T_182; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_184 = _IDU_opcodes_T_15 ? 3'h3 : _IDU_opcodes_T_183; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_185 = _IDU_opcodes_T_13 ? 3'h3 : _IDU_opcodes_T_184; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_186 = _IDU_opcodes_T_11 ? 3'h3 : _IDU_opcodes_T_185; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_187 = _IDU_opcodes_T_9 ? 3'h3 : _IDU_opcodes_T_186; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_188 = _IDU_opcodes_T_7 ? 3'h1 : _IDU_opcodes_T_187; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_189 = _IDU_opcodes_T_5 ? 3'h5 : _IDU_opcodes_T_188; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_190 = _IDU_opcodes_T_3 ? 3'h4 : _IDU_opcodes_T_189; // @[Lookup.scala 34:39]
  wire [2:0] IDU_inst_type = _IDU_opcodes_T_1 ? 3'h4 : _IDU_opcodes_T_190; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_191 = _IDU_opcodes_T_127 ? 6'h3d : 6'h0; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_192 = _IDU_opcodes_T_125 ? 6'h3c : _IDU_opcodes_T_191; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_193 = _IDU_opcodes_T_123 ? 6'h3b : _IDU_opcodes_T_192; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_194 = _IDU_opcodes_T_121 ? 6'h3a : _IDU_opcodes_T_193; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_195 = _IDU_opcodes_T_119 ? 6'h39 : _IDU_opcodes_T_194; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_196 = _IDU_opcodes_T_117 ? 6'h38 : _IDU_opcodes_T_195; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_197 = _IDU_opcodes_T_115 ? 6'h37 : _IDU_opcodes_T_196; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_198 = _IDU_opcodes_T_113 ? 6'h36 : _IDU_opcodes_T_197; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_199 = _IDU_opcodes_T_111 ? 6'h35 : _IDU_opcodes_T_198; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_200 = _IDU_opcodes_T_109 ? 6'h34 : _IDU_opcodes_T_199; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_201 = _IDU_opcodes_T_107 ? 6'h33 : _IDU_opcodes_T_200; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_202 = _IDU_opcodes_T_105 ? 6'h32 : _IDU_opcodes_T_201; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_203 = _IDU_opcodes_T_103 ? 6'h31 : _IDU_opcodes_T_202; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_204 = _IDU_opcodes_T_101 ? 6'h30 : _IDU_opcodes_T_203; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_205 = _IDU_opcodes_T_99 ? 6'h2f : _IDU_opcodes_T_204; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_206 = _IDU_opcodes_T_97 ? 6'h2e : _IDU_opcodes_T_205; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_207 = _IDU_opcodes_T_95 ? 6'h2d : _IDU_opcodes_T_206; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_208 = _IDU_opcodes_T_83 ? 6'h28 : _IDU_opcodes_T_207; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_209 = _IDU_opcodes_T_91 ? 6'h2c : _IDU_opcodes_T_208; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_210 = _IDU_opcodes_T_89 ? 6'h2b : _IDU_opcodes_T_209; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_211 = _IDU_opcodes_T_87 ? 6'h2a : _IDU_opcodes_T_210; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_212 = _IDU_opcodes_T_85 ? 6'h29 : _IDU_opcodes_T_211; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_213 = _IDU_opcodes_T_83 ? 6'h28 : _IDU_opcodes_T_212; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_214 = _IDU_opcodes_T_81 ? 6'h27 : _IDU_opcodes_T_213; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_215 = _IDU_opcodes_T_79 ? 6'h26 : _IDU_opcodes_T_214; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_216 = _IDU_opcodes_T_77 ? 6'h25 : _IDU_opcodes_T_215; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_217 = _IDU_opcodes_T_75 ? 6'h0 : _IDU_opcodes_T_216; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_218 = _IDU_opcodes_T_73 ? 6'h24 : _IDU_opcodes_T_217; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_219 = _IDU_opcodes_T_71 ? 6'h23 : _IDU_opcodes_T_218; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_220 = _IDU_opcodes_T_69 ? 6'h22 : _IDU_opcodes_T_219; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_221 = _IDU_opcodes_T_67 ? 6'h21 : _IDU_opcodes_T_220; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_222 = _IDU_opcodes_T_65 ? 6'h20 : _IDU_opcodes_T_221; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_223 = _IDU_opcodes_T_63 ? 6'h1f : _IDU_opcodes_T_222; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_224 = _IDU_opcodes_T_61 ? 6'h1e : _IDU_opcodes_T_223; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_225 = _IDU_opcodes_T_59 ? 6'h1d : _IDU_opcodes_T_224; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_226 = _IDU_opcodes_T_57 ? 6'h1c : _IDU_opcodes_T_225; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_227 = _IDU_opcodes_T_55 ? 6'h1b : _IDU_opcodes_T_226; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_228 = _IDU_opcodes_T_53 ? 6'h1a : _IDU_opcodes_T_227; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_229 = _IDU_opcodes_T_51 ? 6'h19 : _IDU_opcodes_T_228; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_230 = _IDU_opcodes_T_49 ? 6'h18 : _IDU_opcodes_T_229; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_231 = _IDU_opcodes_T_47 ? 6'h17 : _IDU_opcodes_T_230; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_232 = _IDU_opcodes_T_45 ? 6'h16 : _IDU_opcodes_T_231; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_233 = _IDU_opcodes_T_43 ? 6'h15 : _IDU_opcodes_T_232; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_234 = _IDU_opcodes_T_41 ? 6'h14 : _IDU_opcodes_T_233; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_235 = _IDU_opcodes_T_39 ? 6'h13 : _IDU_opcodes_T_234; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_236 = _IDU_opcodes_T_37 ? 6'h12 : _IDU_opcodes_T_235; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_237 = _IDU_opcodes_T_35 ? 6'h11 : _IDU_opcodes_T_236; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_238 = _IDU_opcodes_T_33 ? 6'h10 : _IDU_opcodes_T_237; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_239 = _IDU_opcodes_T_31 ? 6'hf : _IDU_opcodes_T_238; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_240 = _IDU_opcodes_T_29 ? 6'he : _IDU_opcodes_T_239; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_241 = _IDU_opcodes_T_27 ? 6'hd : _IDU_opcodes_T_240; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_242 = _IDU_opcodes_T_25 ? 6'hc : _IDU_opcodes_T_241; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_243 = _IDU_opcodes_T_23 ? 6'hb : _IDU_opcodes_T_242; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_244 = _IDU_opcodes_T_21 ? 6'ha : _IDU_opcodes_T_243; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_245 = _IDU_opcodes_T_19 ? 6'h9 : _IDU_opcodes_T_244; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_246 = _IDU_opcodes_T_17 ? 6'h8 : _IDU_opcodes_T_245; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_247 = _IDU_opcodes_T_15 ? 6'h7 : _IDU_opcodes_T_246; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_248 = _IDU_opcodes_T_13 ? 6'h3f : _IDU_opcodes_T_247; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_249 = _IDU_opcodes_T_11 ? 6'h6 : _IDU_opcodes_T_248; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_250 = _IDU_opcodes_T_9 ? 6'h5 : _IDU_opcodes_T_249; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_251 = _IDU_opcodes_T_7 ? 6'h4 : _IDU_opcodes_T_250; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_252 = _IDU_opcodes_T_5 ? 6'h3 : _IDU_opcodes_T_251; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_253 = _IDU_opcodes_T_3 ? 6'h2 : _IDU_opcodes_T_252; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_277 = _IDU_opcodes_T_81 ? 4'ha : 4'h0; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_278 = _IDU_opcodes_T_79 ? 4'h6 : _IDU_opcodes_T_277; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_279 = _IDU_opcodes_T_77 ? 4'hb : _IDU_opcodes_T_278; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_280 = _IDU_opcodes_T_75 ? 4'h0 : _IDU_opcodes_T_279; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_281 = _IDU_opcodes_T_73 ? 4'h0 : _IDU_opcodes_T_280; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_282 = _IDU_opcodes_T_71 ? 4'h0 : _IDU_opcodes_T_281; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_283 = _IDU_opcodes_T_69 ? 4'h0 : _IDU_opcodes_T_282; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_284 = _IDU_opcodes_T_67 ? 4'h0 : _IDU_opcodes_T_283; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_285 = _IDU_opcodes_T_65 ? 4'h0 : _IDU_opcodes_T_284; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_286 = _IDU_opcodes_T_63 ? 4'h0 : _IDU_opcodes_T_285; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_287 = _IDU_opcodes_T_61 ? 4'h0 : _IDU_opcodes_T_286; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_288 = _IDU_opcodes_T_59 ? 4'h0 : _IDU_opcodes_T_287; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_289 = _IDU_opcodes_T_57 ? 4'h0 : _IDU_opcodes_T_288; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_290 = _IDU_opcodes_T_55 ? 4'h0 : _IDU_opcodes_T_289; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_291 = _IDU_opcodes_T_53 ? 4'h0 : _IDU_opcodes_T_290; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_292 = _IDU_opcodes_T_51 ? 4'h0 : _IDU_opcodes_T_291; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_293 = _IDU_opcodes_T_49 ? 4'h0 : _IDU_opcodes_T_292; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_294 = _IDU_opcodes_T_47 ? 4'h0 : _IDU_opcodes_T_293; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_295 = _IDU_opcodes_T_45 ? 4'h0 : _IDU_opcodes_T_294; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_296 = _IDU_opcodes_T_43 ? 4'h0 : _IDU_opcodes_T_295; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_297 = _IDU_opcodes_T_41 ? 4'h0 : _IDU_opcodes_T_296; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_298 = _IDU_opcodes_T_39 ? 4'h0 : _IDU_opcodes_T_297; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_299 = _IDU_opcodes_T_37 ? 4'h0 : _IDU_opcodes_T_298; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_300 = _IDU_opcodes_T_35 ? 4'h9 : _IDU_opcodes_T_299; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_301 = _IDU_opcodes_T_33 ? 4'h8 : _IDU_opcodes_T_300; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_302 = _IDU_opcodes_T_31 ? 4'h7 : _IDU_opcodes_T_301; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_303 = _IDU_opcodes_T_29 ? 4'h4 : _IDU_opcodes_T_302; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_304 = _IDU_opcodes_T_27 ? 4'h2 : _IDU_opcodes_T_303; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_305 = _IDU_opcodes_T_25 ? 4'h5 : _IDU_opcodes_T_304; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_306 = _IDU_opcodes_T_23 ? 4'h3 : _IDU_opcodes_T_305; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_307 = _IDU_opcodes_T_21 ? 4'h1 : _IDU_opcodes_T_306; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_308 = _IDU_opcodes_T_19 ? 4'h0 : _IDU_opcodes_T_307; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_309 = _IDU_opcodes_T_17 ? 4'h0 : _IDU_opcodes_T_308; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_310 = _IDU_opcodes_T_15 ? 4'h0 : _IDU_opcodes_T_309; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_311 = _IDU_opcodes_T_13 ? 4'h0 : _IDU_opcodes_T_310; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_312 = _IDU_opcodes_T_11 ? 4'h0 : _IDU_opcodes_T_311; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_313 = _IDU_opcodes_T_9 ? 4'h0 : _IDU_opcodes_T_312; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_314 = _IDU_opcodes_T_7 ? 4'h0 : _IDU_opcodes_T_313; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_315 = _IDU_opcodes_T_5 ? 4'h0 : _IDU_opcodes_T_314; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_316 = _IDU_opcodes_T_3 ? 4'h0 : _IDU_opcodes_T_315; // @[Lookup.scala 34:39]
  wire [3:0] IDU_opcodes_2 = _IDU_opcodes_T_1 ? 4'h0 : _IDU_opcodes_T_316; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_277 = _IDU_opcodes_T_81 ? 1'h0 : _IDU_opcodes_T_83 | (_IDU_opcodes_T_85 | (_IDU_opcodes_T_87 | (
    _IDU_opcodes_T_89 | (_IDU_opcodes_T_91 | (_IDU_opcodes_T_83 | (_IDU_opcodes_T_95 | (_IDU_opcodes_T_97 | (
    _IDU_opcodes_T_99 | (_IDU_opcodes_T_101 | (_IDU_opcodes_T_103 | (_IDU_opcodes_T_105 | (_IDU_opcodes_T_107 | (
    _IDU_opcodes_T_109 | (_IDU_opcodes_T_111 | (_IDU_opcodes_T_113 | (_IDU_opcodes_T_115 | (_IDU_opcodes_T_117 | (
    _IDU_opcodes_T_119 | (_IDU_opcodes_T_121 | (_IDU_opcodes_T_123 | (_IDU_opcodes_T_125 | _IDU_opcodes_T_127)))))))))))
    )))))))))); // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_280 = _IDU_opcodes_T_75 ? 1'h0 : _IDU_opcodes_T_77 | (_IDU_opcodes_T_79 | _IDU_switchs_T_277); // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_300 = _IDU_opcodes_T_35 ? 1'h0 : _IDU_opcodes_T_37 | (_IDU_opcodes_T_39 | (_IDU_opcodes_T_41 | (
    _IDU_opcodes_T_43 | (_IDU_opcodes_T_45 | (_IDU_opcodes_T_47 | (_IDU_opcodes_T_49 | (_IDU_opcodes_T_51 | (
    _IDU_opcodes_T_53 | (_IDU_opcodes_T_55 | (_IDU_opcodes_T_57 | (_IDU_opcodes_T_59 | (_IDU_opcodes_T_61 | (
    _IDU_opcodes_T_63 | (_IDU_opcodes_T_65 | (_IDU_opcodes_T_67 | (_IDU_opcodes_T_69 | (_IDU_opcodes_T_71 | (
    _IDU_opcodes_T_73 | _IDU_switchs_T_280)))))))))))))))))); // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_301 = _IDU_opcodes_T_33 ? 1'h0 : _IDU_switchs_T_300; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_302 = _IDU_opcodes_T_31 ? 1'h0 : _IDU_switchs_T_301; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_308 = _IDU_opcodes_T_19 ? 1'h0 : _IDU_opcodes_T_21 | (_IDU_opcodes_T_23 | (_IDU_opcodes_T_25 | (
    _IDU_opcodes_T_27 | (_IDU_opcodes_T_29 | _IDU_switchs_T_302)))); // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_309 = _IDU_opcodes_T_17 ? 1'h0 : _IDU_switchs_T_308; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_310 = _IDU_opcodes_T_15 ? 1'h0 : _IDU_switchs_T_309; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_311 = _IDU_opcodes_T_13 ? 1'h0 : _IDU_switchs_T_310; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_312 = _IDU_opcodes_T_11 ? 1'h0 : _IDU_switchs_T_311; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_313 = _IDU_opcodes_T_9 ? 1'h0 : _IDU_switchs_T_312; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_317 = _IDU_opcodes_T_127 ? 1'h0 : 1'h1; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_318 = _IDU_opcodes_T_125 ? 1'h0 : _IDU_switchs_T_317; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_319 = _IDU_opcodes_T_123 ? 1'h0 : _IDU_switchs_T_318; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_320 = _IDU_opcodes_T_121 ? 1'h0 : _IDU_switchs_T_319; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_321 = _IDU_opcodes_T_119 ? 1'h0 : _IDU_switchs_T_320; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_322 = _IDU_opcodes_T_117 ? 1'h0 : _IDU_switchs_T_321; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_323 = _IDU_opcodes_T_115 ? 1'h0 : _IDU_switchs_T_322; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_324 = _IDU_opcodes_T_113 ? 1'h0 : _IDU_switchs_T_323; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_325 = _IDU_opcodes_T_111 ? 1'h0 : _IDU_switchs_T_324; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_326 = _IDU_opcodes_T_109 ? 1'h0 : _IDU_switchs_T_325; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_327 = _IDU_opcodes_T_107 ? 1'h0 : _IDU_switchs_T_326; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_328 = _IDU_opcodes_T_105 ? 1'h0 : _IDU_switchs_T_327; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_329 = _IDU_opcodes_T_103 ? 1'h0 : _IDU_switchs_T_328; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_330 = _IDU_opcodes_T_101 ? 1'h0 : _IDU_switchs_T_329; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_331 = _IDU_opcodes_T_99 ? 1'h0 : _IDU_switchs_T_330; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_332 = _IDU_opcodes_T_97 ? 1'h0 : _IDU_switchs_T_331; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_333 = _IDU_opcodes_T_95 ? 1'h0 : _IDU_switchs_T_332; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_334 = _IDU_opcodes_T_83 ? 1'h0 : _IDU_switchs_T_333; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_335 = _IDU_opcodes_T_91 ? 1'h0 : _IDU_switchs_T_334; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_336 = _IDU_opcodes_T_89 ? 1'h0 : _IDU_switchs_T_335; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_337 = _IDU_opcodes_T_87 ? 1'h0 : _IDU_switchs_T_336; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_338 = _IDU_opcodes_T_85 ? 1'h0 : _IDU_switchs_T_337; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_339 = _IDU_opcodes_T_83 ? 1'h0 : _IDU_switchs_T_338; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_340 = _IDU_opcodes_T_81 ? 1'h0 : _IDU_switchs_T_339; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_341 = _IDU_opcodes_T_79 ? 1'h0 : _IDU_switchs_T_340; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_342 = _IDU_opcodes_T_77 ? 1'h0 : _IDU_switchs_T_341; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_407 = _IDU_opcodes_T_73 ? 1'h0 : _IDU_opcodes_T_75 | _IDU_switchs_T_342; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_408 = _IDU_opcodes_T_71 ? 1'h0 : _IDU_switchs_T_407; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_409 = _IDU_opcodes_T_69 ? 1'h0 : _IDU_switchs_T_408; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_410 = _IDU_opcodes_T_67 ? 1'h0 : _IDU_switchs_T_409; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_411 = _IDU_opcodes_T_65 ? 1'h0 : _IDU_switchs_T_410; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_412 = _IDU_opcodes_T_63 ? 1'h0 : _IDU_switchs_T_411; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_413 = _IDU_opcodes_T_61 ? 1'h0 : _IDU_switchs_T_412; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_414 = _IDU_opcodes_T_59 ? 1'h0 : _IDU_switchs_T_413; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_415 = _IDU_opcodes_T_57 ? 1'h0 : _IDU_switchs_T_414; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_416 = _IDU_opcodes_T_55 ? 1'h0 : _IDU_switchs_T_415; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_417 = _IDU_opcodes_T_53 ? 1'h0 : _IDU_switchs_T_416; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_418 = _IDU_opcodes_T_51 ? 1'h0 : _IDU_switchs_T_417; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_419 = _IDU_opcodes_T_49 ? 1'h0 : _IDU_switchs_T_418; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_420 = _IDU_opcodes_T_47 ? 1'h0 : _IDU_switchs_T_419; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_421 = _IDU_opcodes_T_45 ? 1'h0 : _IDU_switchs_T_420; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_422 = _IDU_opcodes_T_43 ? 1'h0 : _IDU_switchs_T_421; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_423 = _IDU_opcodes_T_41 ? 1'h0 : _IDU_switchs_T_422; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_424 = _IDU_opcodes_T_39 ? 1'h0 : _IDU_switchs_T_423; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_425 = _IDU_opcodes_T_37 ? 1'h0 : _IDU_switchs_T_424; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_426 = _IDU_opcodes_T_35 ? 1'h0 : _IDU_switchs_T_425; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_427 = _IDU_opcodes_T_33 ? 1'h0 : _IDU_switchs_T_426; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_428 = _IDU_opcodes_T_31 ? 1'h0 : _IDU_switchs_T_427; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_429 = _IDU_opcodes_T_29 ? 1'h0 : _IDU_switchs_T_428; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_430 = _IDU_opcodes_T_27 ? 1'h0 : _IDU_switchs_T_429; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_431 = _IDU_opcodes_T_25 ? 1'h0 : _IDU_switchs_T_430; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_432 = _IDU_opcodes_T_23 ? 1'h0 : _IDU_switchs_T_431; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_433 = _IDU_opcodes_T_21 ? 1'h0 : _IDU_switchs_T_432; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_434 = _IDU_opcodes_T_19 ? 1'h0 : _IDU_switchs_T_433; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_435 = _IDU_opcodes_T_17 ? 1'h0 : _IDU_switchs_T_434; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_436 = _IDU_opcodes_T_15 ? 1'h0 : _IDU_switchs_T_435; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_437 = _IDU_opcodes_T_13 ? 1'h0 : _IDU_switchs_T_436; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_438 = _IDU_opcodes_T_11 ? 1'h0 : _IDU_switchs_T_437; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_439 = _IDU_opcodes_T_9 ? 1'h0 : _IDU_switchs_T_438; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_440 = _IDU_opcodes_T_7 ? 1'h0 : _IDU_switchs_T_439; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_441 = _IDU_opcodes_T_5 ? 1'h0 : _IDU_switchs_T_440; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_442 = _IDU_opcodes_T_3 ? 1'h0 : _IDU_switchs_T_441; // @[Lookup.scala 34:39]
  wire  _io_IDU_O_imm_T = IDU_inst_type == 3'h7; // @[npc.scala 386:24]
  wire  _io_IDU_O_imm_T_1 = IDU_inst_type == 3'h6; // @[npc.scala 387:24]
  wire  _io_IDU_O_imm_T_2 = IDU_inst_type == 3'h0; // @[npc.scala 388:24]
  wire  _io_IDU_O_imm_T_3 = IDU_inst_type == 3'h1; // @[npc.scala 389:24]
  wire  _io_IDU_O_imm_T_4 = IDU_inst_type == 3'h2; // @[npc.scala 390:24]
  wire  _io_IDU_O_imm_T_5 = IDU_inst_type == 3'h3; // @[npc.scala 391:24]
  wire  _io_IDU_O_imm_T_6 = IDU_inst_type == 3'h4; // @[npc.scala 392:24]
  wire  _io_IDU_O_imm_T_7 = IDU_inst_type == 3'h5; // @[npc.scala 393:24]
  wire [83:0] _io_IDU_O_imm_T_8 = _io_IDU_O_imm_T_7 ? {{20'd0}, SignExtend_immJ} : SignExtend_immR; // @[Mux.scala 101:16]
  wire [83:0] _io_IDU_O_imm_T_9 = _io_IDU_O_imm_T_6 ? {{20'd0}, SignExtend_immU} : _io_IDU_O_imm_T_8; // @[Mux.scala 101:16]
  wire [83:0] _io_IDU_O_imm_T_10 = _io_IDU_O_imm_T_5 ? {{20'd0}, SignExtend_immB} : _io_IDU_O_imm_T_9; // @[Mux.scala 101:16]
  wire [83:0] _io_IDU_O_imm_T_11 = _io_IDU_O_imm_T_4 ? {{20'd0}, SignExtend_immS} : _io_IDU_O_imm_T_10; // @[Mux.scala 101:16]
  wire [83:0] _io_IDU_O_imm_T_12 = _io_IDU_O_imm_T_3 ? {{20'd0}, SignExtend_immI} : _io_IDU_O_imm_T_11; // @[Mux.scala 101:16]
  wire [83:0] _io_IDU_O_imm_T_13 = _io_IDU_O_imm_T_2 ? SignExtend_immR : _io_IDU_O_imm_T_12; // @[Mux.scala 101:16]
  wire [83:0] _io_IDU_O_imm_T_14 = _io_IDU_O_imm_T_1 ? SignExtend_immR : _io_IDU_O_imm_T_13; // @[Mux.scala 101:16]
  wire [83:0] _io_IDU_O_imm_T_15 = _io_IDU_O_imm_T ? SignExtend_immR : _io_IDU_O_imm_T_14; // @[Mux.scala 101:16]
  assign io_IDU_O_rs1 = io_IDU_I_inst[19:15]; // @[npc.scala 213:34]
  assign io_IDU_O_rs2 = io_IDU_I_inst[24:20]; // @[npc.scala 214:34]
  assign io_IDU_O_rd = io_IDU_I_inst[11:7]; // @[npc.scala 215:33]
  assign io_IDU_O_EXUopcode = _IDU_opcodes_T_1 ? 6'h1 : _IDU_opcodes_T_253; // @[Lookup.scala 34:39]
  assign io_IDU_O_LSUopcode = {{2'd0}, IDU_opcodes_2}; // @[npc.scala 307:31]
  assign io_IDU_O_GPRneedWriteBack = _IDU_opcodes_T_1 | (_IDU_opcodes_T_3 | (_IDU_opcodes_T_5 | (_IDU_opcodes_T_7 |
    _IDU_switchs_T_313))); // @[Lookup.scala 34:39]
  assign io_IDU_O_imm = _io_IDU_O_imm_T_15[63:0]; // @[npc.scala 384:18]
  assign io_IDU_O_halt = _IDU_opcodes_T_1 ? 1'h0 : _IDU_switchs_T_442; // @[Lookup.scala 34:39]
endmodule
module EXU(
  input  [63:0] io_EXU_I_src1,
  input  [63:0] io_EXU_I_src2,
  input  [63:0] io_EXU_I_imm,
  input  [5:0]  io_EXU_I_opcode,
  input  [63:0] io_EXU_I_currentPC,
  output [63:0] io_EXU_O_result,
  output [63:0] io_EXU_O_staticNPC,
  output [63:0] io_EXU_O_dynamicNPC
);
  wire [63:0] EXU_src2_signed = io_EXU_I_src2; // @[npc.scala 410:41]
  wire [63:0] _io_EXU_O_staticNPC_T_1 = io_EXU_I_currentPC + 64'h4; // @[npc.scala 415:46]
  wire  _io_EXU_O_result_T = io_EXU_I_opcode == 6'h0; // @[npc.scala 419:26]
  wire  _io_EXU_O_result_T_1 = io_EXU_I_opcode == 6'h1; // @[npc.scala 420:26]
  wire  _io_EXU_O_result_T_2 = io_EXU_I_opcode == 6'h2; // @[npc.scala 421:26]
  wire [63:0] _io_EXU_O_result_T_4 = io_EXU_I_currentPC + io_EXU_I_imm; // @[npc.scala 421:79]
  wire  _io_EXU_O_result_T_5 = io_EXU_I_opcode == 6'h3; // @[npc.scala 422:26]
  wire  _io_EXU_O_result_T_8 = io_EXU_I_opcode == 6'h4; // @[npc.scala 423:26]
  wire  _io_EXU_O_result_T_11 = io_EXU_I_opcode == 6'h5; // @[npc.scala 424:26]
  wire  _io_EXU_O_result_T_14 = io_EXU_I_opcode == 6'h6; // @[npc.scala 425:26]
  wire  _io_EXU_O_result_T_17 = io_EXU_I_opcode == 6'h3f; // @[npc.scala 426:26]
  wire  _io_EXU_O_result_T_20 = io_EXU_I_opcode == 6'h7; // @[npc.scala 427:26]
  wire  _io_EXU_O_result_T_23 = io_EXU_I_opcode == 6'h8; // @[npc.scala 428:26]
  wire  _io_EXU_O_result_T_26 = io_EXU_I_opcode == 6'h9; // @[npc.scala 429:26]
  wire  _io_EXU_O_result_T_29 = io_EXU_I_opcode == 6'ha; // @[npc.scala 430:26]
  wire [63:0] _io_EXU_O_result_T_31 = io_EXU_I_src1 + io_EXU_I_imm; // @[npc.scala 430:78]
  wire  _io_EXU_O_result_T_32 = io_EXU_I_opcode == 6'hb; // @[npc.scala 431:26]
  wire  _io_EXU_O_result_T_35 = io_EXU_I_opcode == 6'hc; // @[npc.scala 432:26]
  wire  _io_EXU_O_result_T_38 = io_EXU_I_opcode == 6'hd; // @[npc.scala 433:26]
  wire  _io_EXU_O_result_T_41 = io_EXU_I_opcode == 6'he; // @[npc.scala 434:26]
  wire  _io_EXU_O_result_T_44 = io_EXU_I_opcode == 6'hf; // @[npc.scala 435:26]
  wire  _io_EXU_O_result_T_47 = io_EXU_I_opcode == 6'h10; // @[npc.scala 436:26]
  wire  _io_EXU_O_result_T_50 = io_EXU_I_opcode == 6'h11; // @[npc.scala 437:26]
  wire  _io_EXU_O_result_T_53 = io_EXU_I_opcode == 6'h12; // @[npc.scala 438:26]
  wire  _io_EXU_O_result_T_56 = io_EXU_I_opcode == 6'h13; // @[npc.scala 439:26]
  wire [63:0] _io_EXU_O_result_T_58 = $signed(io_EXU_I_src1) < $signed(io_EXU_I_imm) ? 64'h1 : 64'h0; // @[npc.scala 439:63]
  wire  _io_EXU_O_result_T_59 = io_EXU_I_opcode == 6'h14; // @[npc.scala 440:26]
  wire [63:0] _io_EXU_O_result_T_61 = io_EXU_I_src1 < io_EXU_I_imm ? 64'h1 : 64'h0; // @[npc.scala 440:63]
  wire  _io_EXU_O_result_T_62 = io_EXU_I_opcode == 6'h15; // @[npc.scala 441:26]
  wire [63:0] _io_EXU_O_result_T_63 = io_EXU_I_src1 ^ io_EXU_I_imm; // @[npc.scala 441:78]
  wire  _io_EXU_O_result_T_64 = io_EXU_I_opcode == 6'h16; // @[npc.scala 442:26]
  wire [63:0] _io_EXU_O_result_T_65 = io_EXU_I_src1 | io_EXU_I_imm; // @[npc.scala 442:78]
  wire  _io_EXU_O_result_T_66 = io_EXU_I_opcode == 6'h17; // @[npc.scala 443:26]
  wire [63:0] _io_EXU_O_result_T_67 = io_EXU_I_src1 & io_EXU_I_imm; // @[npc.scala 443:78]
  wire  _io_EXU_O_result_T_68 = io_EXU_I_opcode == 6'h18; // @[npc.scala 444:26]
  wire [126:0] _GEN_0 = {{63'd0}, io_EXU_I_src1}; // @[npc.scala 444:78]
  wire [126:0] _io_EXU_O_result_T_70 = _GEN_0 << io_EXU_I_imm[5:0]; // @[npc.scala 444:78]
  wire  _io_EXU_O_result_T_71 = io_EXU_I_opcode == 6'h19; // @[npc.scala 445:26]
  wire [63:0] _io_EXU_O_result_T_73 = io_EXU_I_src1 >> io_EXU_I_imm[5:0]; // @[npc.scala 445:78]
  wire  _io_EXU_O_result_T_74 = io_EXU_I_opcode == 6'h1a; // @[npc.scala 446:26]
  wire  _io_EXU_O_result_T_77 = io_EXU_I_opcode == 6'h1b; // @[npc.scala 447:26]
  wire [63:0] _io_EXU_O_result_T_79 = io_EXU_I_src1 + io_EXU_I_src2; // @[npc.scala 447:78]
  wire  _io_EXU_O_result_T_80 = io_EXU_I_opcode == 6'h1c; // @[npc.scala 448:26]
  wire [63:0] _io_EXU_O_result_T_82 = io_EXU_I_src1 - io_EXU_I_src2; // @[npc.scala 448:78]
  wire  _io_EXU_O_result_T_83 = io_EXU_I_opcode == 6'h1d; // @[npc.scala 449:26]
  wire [126:0] _GEN_1 = {{63'd0}, io_EXU_I_src1}; // @[npc.scala 449:78]
  wire [126:0] _io_EXU_O_result_T_85 = _GEN_1 << io_EXU_I_src2[5:0]; // @[npc.scala 449:78]
  wire  _io_EXU_O_result_T_86 = io_EXU_I_opcode == 6'h1e; // @[npc.scala 450:26]
  wire  _io_EXU_O_result_T_87 = $signed(io_EXU_I_src1) < $signed(io_EXU_I_src2); // @[npc.scala 450:80]
  wire [63:0] _io_EXU_O_result_T_88 = $signed(io_EXU_I_src1) < $signed(io_EXU_I_src2) ? 64'h1 : 64'h0; // @[npc.scala 450:63]
  wire  _io_EXU_O_result_T_89 = io_EXU_I_opcode == 6'h1f; // @[npc.scala 451:26]
  wire  _io_EXU_O_result_T_90 = io_EXU_I_src1 < io_EXU_I_src2; // @[npc.scala 451:82]
  wire [63:0] _io_EXU_O_result_T_91 = io_EXU_I_src1 < io_EXU_I_src2 ? 64'h1 : 64'h0; // @[npc.scala 451:63]
  wire  _io_EXU_O_result_T_92 = io_EXU_I_opcode == 6'h20; // @[npc.scala 452:26]
  wire [63:0] _io_EXU_O_result_T_93 = io_EXU_I_src1 ^ io_EXU_I_src2; // @[npc.scala 452:78]
  wire  _io_EXU_O_result_T_94 = io_EXU_I_opcode == 6'h21; // @[npc.scala 453:26]
  wire [63:0] _io_EXU_O_result_T_96 = io_EXU_I_src1 >> io_EXU_I_src2[5:0]; // @[npc.scala 453:78]
  wire  _io_EXU_O_result_T_97 = io_EXU_I_opcode == 6'h22; // @[npc.scala 454:26]
  wire [63:0] _io_EXU_O_result_T_100 = $signed(io_EXU_I_src1) >>> EXU_src2_signed[5:0]; // @[npc.scala 454:102]
  wire  _io_EXU_O_result_T_101 = io_EXU_I_opcode == 6'h23; // @[npc.scala 455:26]
  wire [63:0] _io_EXU_O_result_T_102 = io_EXU_I_src1 | io_EXU_I_src2; // @[npc.scala 455:78]
  wire  _io_EXU_O_result_T_103 = io_EXU_I_opcode == 6'h24; // @[npc.scala 456:26]
  wire [63:0] _io_EXU_O_result_T_104 = io_EXU_I_src1 & io_EXU_I_src2; // @[npc.scala 456:78]
  wire  _io_EXU_O_result_T_105 = io_EXU_I_opcode == 6'h25; // @[npc.scala 457:26]
  wire  _io_EXU_O_result_T_108 = io_EXU_I_opcode == 6'h26; // @[npc.scala 458:26]
  wire  _io_EXU_O_result_T_111 = io_EXU_I_opcode == 6'h27; // @[npc.scala 459:26]
  wire  _io_EXU_O_result_T_114 = io_EXU_I_opcode == 6'h28; // @[npc.scala 460:26]
  wire [31:0] _io_EXU_O_result_T_119 = _io_EXU_O_result_T_31[31] ? 32'hffffffff : 32'h0; // @[Bitwise.scala 77:12]
  wire [63:0] _io_EXU_O_result_T_123 = {_io_EXU_O_result_T_119,_io_EXU_O_result_T_31[31:0]}; // @[Cat.scala 33:92]
  wire  _io_EXU_O_result_T_124 = io_EXU_I_opcode == 6'h29; // @[npc.scala 461:26]
  wire [62:0] _GEN_2 = {{31'd0}, io_EXU_I_src1[31:0]}; // @[npc.scala 461:106]
  wire [62:0] _io_EXU_O_result_T_127 = _GEN_2 << io_EXU_I_imm[4:0]; // @[npc.scala 461:106]
  wire [31:0] _io_EXU_O_result_T_130 = _io_EXU_O_result_T_127[31] ? 32'hffffffff : 32'h0; // @[Bitwise.scala 77:12]
  wire [63:0] _io_EXU_O_result_T_135 = {_io_EXU_O_result_T_130,_io_EXU_O_result_T_127[31:0]}; // @[Cat.scala 33:92]
  wire  _io_EXU_O_result_T_136 = io_EXU_I_opcode == 6'h2a; // @[npc.scala 462:26]
  wire [31:0] _io_EXU_O_result_T_139 = io_EXU_I_src1[31:0] >> io_EXU_I_imm[4:0]; // @[npc.scala 462:106]
  wire [31:0] _io_EXU_O_result_T_142 = _io_EXU_O_result_T_139[31] ? 32'hffffffff : 32'h0; // @[Bitwise.scala 77:12]
  wire [63:0] _io_EXU_O_result_T_147 = {_io_EXU_O_result_T_142,_io_EXU_O_result_T_139}; // @[Cat.scala 33:92]
  wire  _io_EXU_O_result_T_148 = io_EXU_I_opcode == 6'h2b; // @[npc.scala 463:26]
  wire [31:0] _io_EXU_O_result_T_150 = io_EXU_I_src1[31:0]; // @[npc.scala 463:99]
  wire [31:0] _io_EXU_O_result_T_152 = $signed(_io_EXU_O_result_T_150) >>> io_EXU_I_imm[4:0]; // @[npc.scala 463:106]
  wire [31:0] _io_EXU_O_result_T_155 = _io_EXU_O_result_T_152[31] ? 32'hffffffff : 32'h0; // @[Bitwise.scala 77:12]
  wire [31:0] _io_EXU_O_result_T_160 = $signed(_io_EXU_O_result_T_150) >>> io_EXU_I_imm[4:0]; // @[npc.scala 463:212]
  wire [63:0] _io_EXU_O_result_T_161 = {_io_EXU_O_result_T_155,_io_EXU_O_result_T_160}; // @[Cat.scala 33:92]
  wire  _io_EXU_O_result_T_162 = io_EXU_I_opcode == 6'h2c; // @[npc.scala 467:26]
  wire [31:0] _io_EXU_O_result_T_167 = _io_EXU_O_result_T_79[31] ? 32'hffffffff : 32'h0; // @[Bitwise.scala 77:12]
  wire [63:0] _io_EXU_O_result_T_171 = {_io_EXU_O_result_T_167,_io_EXU_O_result_T_79[31:0]}; // @[Cat.scala 33:92]
  wire  _io_EXU_O_result_T_172 = io_EXU_I_opcode == 6'h2d; // @[npc.scala 468:26]
  wire [31:0] _io_EXU_O_result_T_177 = _io_EXU_O_result_T_82[31] ? 32'hffffffff : 32'h0; // @[Bitwise.scala 77:12]
  wire [63:0] _io_EXU_O_result_T_181 = {_io_EXU_O_result_T_177,_io_EXU_O_result_T_82[31:0]}; // @[Cat.scala 33:92]
  wire  _io_EXU_O_result_T_182 = io_EXU_I_opcode == 6'h2e; // @[npc.scala 470:26]
  wire [62:0] _GEN_3 = {{31'd0}, io_EXU_I_src1[31:0]}; // @[npc.scala 470:106]
  wire [62:0] _io_EXU_O_result_T_185 = _GEN_3 << io_EXU_I_src2[4:0]; // @[npc.scala 470:106]
  wire [31:0] _io_EXU_O_result_T_188 = _io_EXU_O_result_T_185[31] ? 32'hffffffff : 32'h0; // @[Bitwise.scala 77:12]
  wire [63:0] _io_EXU_O_result_T_193 = {_io_EXU_O_result_T_188,_io_EXU_O_result_T_185[31:0]}; // @[Cat.scala 33:92]
  wire  _io_EXU_O_result_T_194 = io_EXU_I_opcode == 6'h2f; // @[npc.scala 472:26]
  wire [31:0] _io_EXU_O_result_T_197 = io_EXU_I_src1[31:0] >> io_EXU_I_src2[4:0]; // @[npc.scala 472:106]
  wire [31:0] _io_EXU_O_result_T_200 = _io_EXU_O_result_T_197[31] ? 32'hffffffff : 32'h0; // @[Bitwise.scala 77:12]
  wire [63:0] _io_EXU_O_result_T_205 = {_io_EXU_O_result_T_200,_io_EXU_O_result_T_197}; // @[Cat.scala 33:92]
  wire  _io_EXU_O_result_T_206 = io_EXU_I_opcode == 6'h30; // @[npc.scala 473:26]
  wire [31:0] _io_EXU_O_result_T_210 = $signed(_io_EXU_O_result_T_150) >>> io_EXU_I_src2[4:0]; // @[npc.scala 473:106]
  wire [31:0] _io_EXU_O_result_T_213 = _io_EXU_O_result_T_210[31] ? 32'hffffffff : 32'h0; // @[Bitwise.scala 77:12]
  wire [31:0] _io_EXU_O_result_T_218 = $signed(_io_EXU_O_result_T_150) >>> io_EXU_I_src2[4:0]; // @[npc.scala 473:214]
  wire [63:0] _io_EXU_O_result_T_219 = {_io_EXU_O_result_T_213,_io_EXU_O_result_T_218}; // @[Cat.scala 33:92]
  wire  _io_EXU_O_result_T_220 = io_EXU_I_opcode == 6'h31; // @[npc.scala 475:26]
  wire [127:0] _io_EXU_O_result_T_221 = io_EXU_I_src1 * io_EXU_I_src2; // @[npc.scala 475:79]
  wire  _io_EXU_O_result_T_223 = io_EXU_I_opcode == 6'h32; // @[npc.scala 476:26]
  wire [127:0] _io_EXU_O_result_T_224 = $signed(io_EXU_I_src1) * $signed(io_EXU_I_src2); // @[npc.scala 476:77]
  wire  _io_EXU_O_result_T_226 = io_EXU_I_opcode == 6'h33; // @[npc.scala 477:26]
  wire [64:0] _io_EXU_O_result_T_227 = {1'b0,$signed(io_EXU_I_src2)}; // @[npc.scala 477:77]
  wire [128:0] _io_EXU_O_result_T_228 = $signed(io_EXU_I_src1) * $signed(_io_EXU_O_result_T_227); // @[npc.scala 477:77]
  wire [127:0] _io_EXU_O_result_T_230 = _io_EXU_O_result_T_228[127:0]; // @[npc.scala 477:77]
  wire  _io_EXU_O_result_T_232 = io_EXU_I_opcode == 6'h34; // @[npc.scala 478:26]
  wire  _io_EXU_O_result_T_235 = io_EXU_I_opcode == 6'h35; // @[npc.scala 479:26]
  wire [64:0] _io_EXU_O_result_T_236 = $signed(io_EXU_I_src1) / $signed(io_EXU_I_src2); // @[npc.scala 479:77]
  wire [64:0] _io_EXU_O_result_T_237 = $signed(io_EXU_I_src1) / $signed(io_EXU_I_src2); // @[npc.scala 479:97]
  wire  _io_EXU_O_result_T_238 = io_EXU_I_opcode == 6'h36; // @[npc.scala 480:26]
  wire [63:0] _io_EXU_O_result_T_239 = io_EXU_I_src1 / io_EXU_I_src2; // @[npc.scala 480:79]
  wire  _io_EXU_O_result_T_240 = io_EXU_I_opcode == 6'h37; // @[npc.scala 481:26]
  wire [63:0] _io_EXU_O_result_T_241 = $signed(io_EXU_I_src1) % $signed(io_EXU_I_src2); // @[npc.scala 481:77]
  wire [63:0] _io_EXU_O_result_T_242 = $signed(io_EXU_I_src1) % $signed(io_EXU_I_src2); // @[npc.scala 481:97]
  wire  _io_EXU_O_result_T_243 = io_EXU_I_opcode == 6'h38; // @[npc.scala 482:26]
  wire [63:0] _io_EXU_O_result_T_244 = io_EXU_I_src1 % io_EXU_I_src2; // @[npc.scala 482:79]
  wire  _io_EXU_O_result_T_245 = io_EXU_I_opcode == 6'h39; // @[npc.scala 483:26]
  wire [31:0] _io_EXU_O_result_T_249 = _io_EXU_O_result_T_221[31] ? 32'hffffffff : 32'h0; // @[Bitwise.scala 77:12]
  wire [63:0] _io_EXU_O_result_T_252 = {_io_EXU_O_result_T_249,_io_EXU_O_result_T_221[31:0]}; // @[Cat.scala 33:92]
  wire  _io_EXU_O_result_T_253 = io_EXU_I_opcode == 6'h3a; // @[npc.scala 484:26]
  wire [31:0] _io_EXU_O_result_T_257 = _io_EXU_O_result_T_236[31] ? 32'hffffffff : 32'h0; // @[Bitwise.scala 77:12]
  wire [63:0] _io_EXU_O_result_T_260 = {_io_EXU_O_result_T_257,_io_EXU_O_result_T_236[31:0]}; // @[Cat.scala 33:92]
  wire  _io_EXU_O_result_T_261 = io_EXU_I_opcode == 6'h3b; // @[npc.scala 485:26]
  wire [31:0] _io_EXU_O_result_T_265 = _io_EXU_O_result_T_239[31] ? 32'hffffffff : 32'h0; // @[Bitwise.scala 77:12]
  wire [63:0] _io_EXU_O_result_T_268 = {_io_EXU_O_result_T_265,_io_EXU_O_result_T_239[31:0]}; // @[Cat.scala 33:92]
  wire  _io_EXU_O_result_T_269 = io_EXU_I_opcode == 6'h3c; // @[npc.scala 486:26]
  wire [31:0] _io_EXU_O_result_T_273 = _io_EXU_O_result_T_241[31] ? 32'hffffffff : 32'h0; // @[Bitwise.scala 77:12]
  wire [63:0] _io_EXU_O_result_T_276 = {_io_EXU_O_result_T_273,_io_EXU_O_result_T_241[31:0]}; // @[Cat.scala 33:92]
  wire  _io_EXU_O_result_T_277 = io_EXU_I_opcode == 6'h3d; // @[npc.scala 487:26]
  wire [31:0] _io_EXU_O_result_T_281 = _io_EXU_O_result_T_244[31] ? 32'hffffffff : 32'h0; // @[Bitwise.scala 77:12]
  wire [63:0] _io_EXU_O_result_T_284 = {_io_EXU_O_result_T_281,_io_EXU_O_result_T_244[31:0]}; // @[Cat.scala 33:92]
  wire [63:0] _io_EXU_O_result_T_285 = _io_EXU_O_result_T_277 ? _io_EXU_O_result_T_284 : 64'h0; // @[Mux.scala 101:16]
  wire [63:0] _io_EXU_O_result_T_286 = _io_EXU_O_result_T_269 ? _io_EXU_O_result_T_276 : _io_EXU_O_result_T_285; // @[Mux.scala 101:16]
  wire [63:0] _io_EXU_O_result_T_287 = _io_EXU_O_result_T_261 ? _io_EXU_O_result_T_268 : _io_EXU_O_result_T_286; // @[Mux.scala 101:16]
  wire [63:0] _io_EXU_O_result_T_288 = _io_EXU_O_result_T_253 ? _io_EXU_O_result_T_260 : _io_EXU_O_result_T_287; // @[Mux.scala 101:16]
  wire [63:0] _io_EXU_O_result_T_289 = _io_EXU_O_result_T_245 ? _io_EXU_O_result_T_252 : _io_EXU_O_result_T_288; // @[Mux.scala 101:16]
  wire [63:0] _io_EXU_O_result_T_290 = _io_EXU_O_result_T_243 ? _io_EXU_O_result_T_244 : _io_EXU_O_result_T_289; // @[Mux.scala 101:16]
  wire [63:0] _io_EXU_O_result_T_291 = _io_EXU_O_result_T_240 ? _io_EXU_O_result_T_242 : _io_EXU_O_result_T_290; // @[Mux.scala 101:16]
  wire [63:0] _io_EXU_O_result_T_292 = _io_EXU_O_result_T_238 ? _io_EXU_O_result_T_239 : _io_EXU_O_result_T_291; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_293 = _io_EXU_O_result_T_235 ? _io_EXU_O_result_T_237 : {{1'd0}, _io_EXU_O_result_T_292
    }; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_294 = _io_EXU_O_result_T_232 ? {{1'd0}, _io_EXU_O_result_T_221[127:64]} :
    _io_EXU_O_result_T_293; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_295 = _io_EXU_O_result_T_226 ? {{1'd0}, _io_EXU_O_result_T_230[127:64]} :
    _io_EXU_O_result_T_294; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_296 = _io_EXU_O_result_T_223 ? {{1'd0}, _io_EXU_O_result_T_224[127:64]} :
    _io_EXU_O_result_T_295; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_297 = _io_EXU_O_result_T_220 ? {{1'd0}, _io_EXU_O_result_T_221[63:0]} :
    _io_EXU_O_result_T_296; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_298 = _io_EXU_O_result_T_206 ? {{1'd0}, _io_EXU_O_result_T_219} :
    _io_EXU_O_result_T_297; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_299 = _io_EXU_O_result_T_194 ? {{1'd0}, _io_EXU_O_result_T_205} :
    _io_EXU_O_result_T_298; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_300 = _io_EXU_O_result_T_182 ? {{1'd0}, _io_EXU_O_result_T_193} :
    _io_EXU_O_result_T_299; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_301 = _io_EXU_O_result_T_172 ? {{1'd0}, _io_EXU_O_result_T_181} :
    _io_EXU_O_result_T_300; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_302 = _io_EXU_O_result_T_162 ? {{1'd0}, _io_EXU_O_result_T_171} :
    _io_EXU_O_result_T_301; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_303 = _io_EXU_O_result_T_148 ? {{1'd0}, _io_EXU_O_result_T_161} :
    _io_EXU_O_result_T_302; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_304 = _io_EXU_O_result_T_136 ? {{1'd0}, _io_EXU_O_result_T_147} :
    _io_EXU_O_result_T_303; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_305 = _io_EXU_O_result_T_124 ? {{1'd0}, _io_EXU_O_result_T_135} :
    _io_EXU_O_result_T_304; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_306 = _io_EXU_O_result_T_114 ? {{1'd0}, _io_EXU_O_result_T_123} :
    _io_EXU_O_result_T_305; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_307 = _io_EXU_O_result_T_111 ? {{1'd0}, _io_EXU_O_result_T_31} : _io_EXU_O_result_T_306
    ; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_308 = _io_EXU_O_result_T_108 ? {{1'd0}, _io_EXU_O_result_T_31} : _io_EXU_O_result_T_307
    ; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_309 = _io_EXU_O_result_T_105 ? {{1'd0}, _io_EXU_O_result_T_31} : _io_EXU_O_result_T_308
    ; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_310 = _io_EXU_O_result_T_103 ? {{1'd0}, _io_EXU_O_result_T_104} :
    _io_EXU_O_result_T_309; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_311 = _io_EXU_O_result_T_101 ? {{1'd0}, _io_EXU_O_result_T_102} :
    _io_EXU_O_result_T_310; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_312 = _io_EXU_O_result_T_97 ? {{1'd0}, _io_EXU_O_result_T_100} : _io_EXU_O_result_T_311
    ; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_313 = _io_EXU_O_result_T_94 ? {{1'd0}, _io_EXU_O_result_T_96} : _io_EXU_O_result_T_312; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_314 = _io_EXU_O_result_T_92 ? {{1'd0}, _io_EXU_O_result_T_93} : _io_EXU_O_result_T_313; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_315 = _io_EXU_O_result_T_89 ? {{1'd0}, _io_EXU_O_result_T_91} : _io_EXU_O_result_T_314; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_316 = _io_EXU_O_result_T_86 ? {{1'd0}, _io_EXU_O_result_T_88} : _io_EXU_O_result_T_315; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_317 = _io_EXU_O_result_T_83 ? _io_EXU_O_result_T_85 : {{62'd0}, _io_EXU_O_result_T_316
    }; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_318 = _io_EXU_O_result_T_80 ? {{63'd0}, _io_EXU_O_result_T_82} :
    _io_EXU_O_result_T_317; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_319 = _io_EXU_O_result_T_77 ? {{63'd0}, _io_EXU_O_result_T_79} :
    _io_EXU_O_result_T_318; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_320 = _io_EXU_O_result_T_74 ? {{63'd0}, _io_EXU_O_result_T_73} :
    _io_EXU_O_result_T_319; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_321 = _io_EXU_O_result_T_71 ? {{63'd0}, _io_EXU_O_result_T_73} :
    _io_EXU_O_result_T_320; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_322 = _io_EXU_O_result_T_68 ? _io_EXU_O_result_T_70 : _io_EXU_O_result_T_321; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_323 = _io_EXU_O_result_T_66 ? {{63'd0}, _io_EXU_O_result_T_67} :
    _io_EXU_O_result_T_322; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_324 = _io_EXU_O_result_T_64 ? {{63'd0}, _io_EXU_O_result_T_65} :
    _io_EXU_O_result_T_323; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_325 = _io_EXU_O_result_T_62 ? {{63'd0}, _io_EXU_O_result_T_63} :
    _io_EXU_O_result_T_324; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_326 = _io_EXU_O_result_T_59 ? {{63'd0}, _io_EXU_O_result_T_61} :
    _io_EXU_O_result_T_325; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_327 = _io_EXU_O_result_T_56 ? {{63'd0}, _io_EXU_O_result_T_58} :
    _io_EXU_O_result_T_326; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_328 = _io_EXU_O_result_T_53 ? {{63'd0}, _io_EXU_O_result_T_31} :
    _io_EXU_O_result_T_327; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_329 = _io_EXU_O_result_T_50 ? {{63'd0}, _io_EXU_O_result_T_31} :
    _io_EXU_O_result_T_328; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_330 = _io_EXU_O_result_T_47 ? {{63'd0}, _io_EXU_O_result_T_31} :
    _io_EXU_O_result_T_329; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_331 = _io_EXU_O_result_T_44 ? {{63'd0}, _io_EXU_O_result_T_31} :
    _io_EXU_O_result_T_330; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_332 = _io_EXU_O_result_T_41 ? {{63'd0}, _io_EXU_O_result_T_31} :
    _io_EXU_O_result_T_331; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_333 = _io_EXU_O_result_T_38 ? {{63'd0}, _io_EXU_O_result_T_31} :
    _io_EXU_O_result_T_332; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_334 = _io_EXU_O_result_T_35 ? {{63'd0}, _io_EXU_O_result_T_31} :
    _io_EXU_O_result_T_333; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_335 = _io_EXU_O_result_T_32 ? {{63'd0}, _io_EXU_O_result_T_31} :
    _io_EXU_O_result_T_334; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_336 = _io_EXU_O_result_T_29 ? {{63'd0}, _io_EXU_O_result_T_31} :
    _io_EXU_O_result_T_335; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_337 = _io_EXU_O_result_T_26 ? {{63'd0}, _io_EXU_O_result_T_4} : _io_EXU_O_result_T_336
    ; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_338 = _io_EXU_O_result_T_23 ? {{63'd0}, _io_EXU_O_result_T_4} : _io_EXU_O_result_T_337
    ; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_339 = _io_EXU_O_result_T_20 ? {{63'd0}, _io_EXU_O_result_T_4} : _io_EXU_O_result_T_338
    ; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_340 = _io_EXU_O_result_T_17 ? {{63'd0}, _io_EXU_O_result_T_4} : _io_EXU_O_result_T_339
    ; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_341 = _io_EXU_O_result_T_14 ? {{63'd0}, _io_EXU_O_result_T_4} : _io_EXU_O_result_T_340
    ; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_342 = _io_EXU_O_result_T_11 ? {{63'd0}, _io_EXU_O_result_T_4} : _io_EXU_O_result_T_341
    ; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_343 = _io_EXU_O_result_T_8 ? {{63'd0}, _io_EXU_O_staticNPC_T_1} :
    _io_EXU_O_result_T_342; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_344 = _io_EXU_O_result_T_5 ? {{63'd0}, _io_EXU_O_staticNPC_T_1} :
    _io_EXU_O_result_T_343; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_345 = _io_EXU_O_result_T_2 ? {{63'd0}, _io_EXU_O_result_T_4} : _io_EXU_O_result_T_344; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_346 = _io_EXU_O_result_T_1 ? {{63'd0}, io_EXU_I_imm} : _io_EXU_O_result_T_345; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_347 = _io_EXU_O_result_T ? 127'h0 : _io_EXU_O_result_T_346; // @[Mux.scala 101:16]
  wire [63:0] _io_EXU_O_dynamicNPC_T_6 = _io_EXU_O_result_T_31 & 64'hfffffffffffffffe; // @[npc.scala 495:96]
  wire [63:0] _io_EXU_O_dynamicNPC_T_13 = io_EXU_I_src1 == io_EXU_I_src2 ? _io_EXU_O_result_T_4 :
    _io_EXU_O_staticNPC_T_1; // @[npc.scala 496:63]
  wire [63:0] _io_EXU_O_dynamicNPC_T_20 = io_EXU_I_src1 != io_EXU_I_src2 ? _io_EXU_O_result_T_4 :
    _io_EXU_O_staticNPC_T_1; // @[npc.scala 497:63]
  wire [63:0] _io_EXU_O_dynamicNPC_T_27 = _io_EXU_O_result_T_87 ? _io_EXU_O_result_T_4 : _io_EXU_O_staticNPC_T_1; // @[npc.scala 498:63]
  wire [63:0] _io_EXU_O_dynamicNPC_T_34 = $signed(io_EXU_I_src1) >= $signed(io_EXU_I_src2) ? _io_EXU_O_result_T_4 :
    _io_EXU_O_staticNPC_T_1; // @[npc.scala 499:63]
  wire [63:0] _io_EXU_O_dynamicNPC_T_41 = _io_EXU_O_result_T_90 ? _io_EXU_O_result_T_4 : _io_EXU_O_staticNPC_T_1; // @[npc.scala 500:63]
  wire [63:0] _io_EXU_O_dynamicNPC_T_48 = io_EXU_I_src1 >= io_EXU_I_src2 ? _io_EXU_O_result_T_4 :
    _io_EXU_O_staticNPC_T_1; // @[npc.scala 501:63]
  wire [63:0] _io_EXU_O_dynamicNPC_T_49 = _io_EXU_O_result_T_26 ? _io_EXU_O_dynamicNPC_T_48 : io_EXU_O_staticNPC; // @[Mux.scala 101:16]
  wire [63:0] _io_EXU_O_dynamicNPC_T_50 = _io_EXU_O_result_T_23 ? _io_EXU_O_dynamicNPC_T_41 : _io_EXU_O_dynamicNPC_T_49; // @[Mux.scala 101:16]
  wire [63:0] _io_EXU_O_dynamicNPC_T_51 = _io_EXU_O_result_T_20 ? _io_EXU_O_dynamicNPC_T_34 : _io_EXU_O_dynamicNPC_T_50; // @[Mux.scala 101:16]
  wire [63:0] _io_EXU_O_dynamicNPC_T_52 = _io_EXU_O_result_T_17 ? _io_EXU_O_dynamicNPC_T_27 : _io_EXU_O_dynamicNPC_T_51; // @[Mux.scala 101:16]
  wire [63:0] _io_EXU_O_dynamicNPC_T_53 = _io_EXU_O_result_T_14 ? _io_EXU_O_dynamicNPC_T_20 : _io_EXU_O_dynamicNPC_T_52; // @[Mux.scala 101:16]
  wire [63:0] _io_EXU_O_dynamicNPC_T_54 = _io_EXU_O_result_T_11 ? _io_EXU_O_dynamicNPC_T_13 : _io_EXU_O_dynamicNPC_T_53; // @[Mux.scala 101:16]
  wire [63:0] _io_EXU_O_dynamicNPC_T_55 = _io_EXU_O_result_T_8 ? _io_EXU_O_dynamicNPC_T_6 : _io_EXU_O_dynamicNPC_T_54; // @[Mux.scala 101:16]
  assign io_EXU_O_result = _io_EXU_O_result_T_347[63:0]; // @[npc.scala 417:21]
  assign io_EXU_O_staticNPC = io_EXU_I_currentPC + 64'h4; // @[npc.scala 415:46]
  assign io_EXU_O_dynamicNPC = _io_EXU_O_result_T_5 ? _io_EXU_O_result_T_4 : _io_EXU_O_dynamicNPC_T_55; // @[Mux.scala 101:16]
endmodule
module LSU(
  input  [63:0] io_LSU_I_src1,
  input  [63:0] io_LSU_I_src2,
  input  [5:0]  io_LSU_I_opcode,
  output [63:0] io_LSU_O_result,
  output [63:0] io_LSU_O_memAddr,
  output        io_LSU_O_accessMem,
  output        io_LSU_O_memRW,
  input  [63:0] io_LSU_I_memR,
  output [63:0] io_LSU_O_memW,
  output [1:0]  io_LSU_O_len
);
  wire [55:0] _LSU_LB_Mask_T_2 = io_LSU_I_memR[7] ? 56'hffffffffffffff : 56'h0; // @[Bitwise.scala 77:12]
  wire [63:0] LSU_LB_Mask = {_LSU_LB_Mask_T_2,8'h0}; // @[Cat.scala 33:92]
  wire [47:0] _LSU_LH_Mask_T_2 = io_LSU_I_memR[15] ? 48'hffffffffffff : 48'h0; // @[Bitwise.scala 77:12]
  wire [63:0] LSU_LH_Mask = {_LSU_LH_Mask_T_2,16'h0}; // @[Cat.scala 33:92]
  wire [31:0] _LSU_LW_Mask_T_2 = io_LSU_I_memR[31] ? 32'hffffffff : 32'h0; // @[Bitwise.scala 77:12]
  wire [63:0] LSU_LW_Mask = {_LSU_LW_Mask_T_2,32'h0}; // @[Cat.scala 33:92]
  wire  _io_LSU_O_result_T = io_LSU_I_opcode == 6'h0; // @[npc.scala 529:26]
  wire  _io_LSU_O_result_T_1 = io_LSU_I_opcode == 6'h1; // @[npc.scala 531:26]
  wire [63:0] _io_LSU_O_result_T_2 = io_LSU_I_memR | LSU_LB_Mask; // @[npc.scala 531:74]
  wire  _io_LSU_O_result_T_3 = io_LSU_I_opcode == 6'h2; // @[npc.scala 532:26]
  wire [119:0] _io_LSU_O_result_T_5 = {56'h0,io_LSU_I_memR}; // @[Cat.scala 33:92]
  wire  _io_LSU_O_result_T_6 = io_LSU_I_opcode == 6'h3; // @[npc.scala 534:26]
  wire [63:0] _io_LSU_O_result_T_7 = io_LSU_I_memR | LSU_LH_Mask; // @[npc.scala 534:74]
  wire  _io_LSU_O_result_T_8 = io_LSU_I_opcode == 6'h4; // @[npc.scala 535:26]
  wire [111:0] _io_LSU_O_result_T_10 = {48'h0,io_LSU_I_memR}; // @[Cat.scala 33:92]
  wire  _io_LSU_O_result_T_11 = io_LSU_I_opcode == 6'h5; // @[npc.scala 537:26]
  wire [63:0] _io_LSU_O_result_T_12 = io_LSU_I_memR | LSU_LW_Mask; // @[npc.scala 537:74]
  wire  _io_LSU_O_result_T_13 = io_LSU_I_opcode == 6'hb; // @[npc.scala 538:26]
  wire [95:0] _io_LSU_O_result_T_15 = {32'h0,io_LSU_I_memR}; // @[Cat.scala 33:92]
  wire  _io_LSU_O_result_T_16 = io_LSU_I_opcode == 6'h6; // @[npc.scala 539:26]
  wire  _io_LSU_O_result_T_17 = io_LSU_I_opcode == 6'h7; // @[npc.scala 540:26]
  wire  _io_LSU_O_result_T_18 = io_LSU_I_opcode == 6'h8; // @[npc.scala 541:26]
  wire  _io_LSU_O_result_T_19 = io_LSU_I_opcode == 6'h9; // @[npc.scala 542:26]
  wire  _io_LSU_O_result_T_20 = io_LSU_I_opcode == 6'ha; // @[npc.scala 543:26]
  wire [63:0] _io_LSU_O_result_T_25 = _io_LSU_O_result_T_16 ? io_LSU_I_memR : 64'h0; // @[Mux.scala 101:16]
  wire [95:0] _io_LSU_O_result_T_26 = _io_LSU_O_result_T_13 ? _io_LSU_O_result_T_15 : {{32'd0}, _io_LSU_O_result_T_25}; // @[Mux.scala 101:16]
  wire [95:0] _io_LSU_O_result_T_27 = _io_LSU_O_result_T_11 ? {{32'd0}, _io_LSU_O_result_T_12} : _io_LSU_O_result_T_26; // @[Mux.scala 101:16]
  wire [111:0] _io_LSU_O_result_T_28 = _io_LSU_O_result_T_8 ? _io_LSU_O_result_T_10 : {{16'd0}, _io_LSU_O_result_T_27}; // @[Mux.scala 101:16]
  wire [111:0] _io_LSU_O_result_T_29 = _io_LSU_O_result_T_6 ? {{48'd0}, _io_LSU_O_result_T_7} : _io_LSU_O_result_T_28; // @[Mux.scala 101:16]
  wire [119:0] _io_LSU_O_result_T_30 = _io_LSU_O_result_T_3 ? _io_LSU_O_result_T_5 : {{8'd0}, _io_LSU_O_result_T_29}; // @[Mux.scala 101:16]
  wire [119:0] _io_LSU_O_result_T_31 = _io_LSU_O_result_T_1 ? {{56'd0}, _io_LSU_O_result_T_2} : _io_LSU_O_result_T_30; // @[Mux.scala 101:16]
  wire [119:0] _io_LSU_O_result_T_32 = _io_LSU_O_result_T ? 120'h0 : _io_LSU_O_result_T_31; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memAddr_T_12 = _io_LSU_O_result_T_20 ? io_LSU_I_src1 : 64'h0; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memAddr_T_13 = _io_LSU_O_result_T_19 ? io_LSU_I_src1 : _io_LSU_O_memAddr_T_12; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memAddr_T_14 = _io_LSU_O_result_T_18 ? io_LSU_I_src1 : _io_LSU_O_memAddr_T_13; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memAddr_T_15 = _io_LSU_O_result_T_17 ? io_LSU_I_src1 : _io_LSU_O_memAddr_T_14; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memAddr_T_16 = _io_LSU_O_result_T_16 ? io_LSU_I_src1 : _io_LSU_O_memAddr_T_15; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memAddr_T_17 = _io_LSU_O_result_T_13 ? io_LSU_I_src1 : _io_LSU_O_memAddr_T_16; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memAddr_T_18 = _io_LSU_O_result_T_11 ? io_LSU_I_src1 : _io_LSU_O_memAddr_T_17; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memAddr_T_19 = _io_LSU_O_result_T_8 ? io_LSU_I_src1 : _io_LSU_O_memAddr_T_18; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memAddr_T_20 = _io_LSU_O_result_T_6 ? io_LSU_I_src1 : _io_LSU_O_memAddr_T_19; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memAddr_T_21 = _io_LSU_O_result_T_3 ? io_LSU_I_src1 : _io_LSU_O_memAddr_T_20; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memAddr_T_22 = _io_LSU_O_result_T_1 ? io_LSU_I_src1 : _io_LSU_O_memAddr_T_21; // @[Mux.scala 101:16]
  wire  _io_LSU_O_memRW_T_16 = _io_LSU_O_result_T_16 ? 1'h0 : _io_LSU_O_result_T_17 | (_io_LSU_O_result_T_18 | (
    _io_LSU_O_result_T_19 | _io_LSU_O_result_T_20)); // @[Mux.scala 101:16]
  wire  _io_LSU_O_memRW_T_17 = _io_LSU_O_result_T_13 ? 1'h0 : _io_LSU_O_memRW_T_16; // @[Mux.scala 101:16]
  wire  _io_LSU_O_memRW_T_18 = _io_LSU_O_result_T_11 ? 1'h0 : _io_LSU_O_memRW_T_17; // @[Mux.scala 101:16]
  wire  _io_LSU_O_memRW_T_19 = _io_LSU_O_result_T_8 ? 1'h0 : _io_LSU_O_memRW_T_18; // @[Mux.scala 101:16]
  wire  _io_LSU_O_memRW_T_20 = _io_LSU_O_result_T_6 ? 1'h0 : _io_LSU_O_memRW_T_19; // @[Mux.scala 101:16]
  wire  _io_LSU_O_memRW_T_21 = _io_LSU_O_result_T_3 ? 1'h0 : _io_LSU_O_memRW_T_20; // @[Mux.scala 101:16]
  wire  _io_LSU_O_memRW_T_22 = _io_LSU_O_result_T_1 ? 1'h0 : _io_LSU_O_memRW_T_21; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memW_T_12 = _io_LSU_O_result_T_20 ? io_LSU_I_src2 : 64'h0; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memW_T_13 = _io_LSU_O_result_T_19 ? io_LSU_I_src2 : _io_LSU_O_memW_T_12; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memW_T_14 = _io_LSU_O_result_T_18 ? io_LSU_I_src2 : _io_LSU_O_memW_T_13; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memW_T_15 = _io_LSU_O_result_T_17 ? io_LSU_I_src2 : _io_LSU_O_memW_T_14; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memW_T_16 = _io_LSU_O_result_T_16 ? 64'h0 : _io_LSU_O_memW_T_15; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memW_T_17 = _io_LSU_O_result_T_13 ? 64'h0 : _io_LSU_O_memW_T_16; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memW_T_18 = _io_LSU_O_result_T_11 ? 64'h0 : _io_LSU_O_memW_T_17; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memW_T_19 = _io_LSU_O_result_T_8 ? 64'h0 : _io_LSU_O_memW_T_18; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memW_T_20 = _io_LSU_O_result_T_6 ? 64'h0 : _io_LSU_O_memW_T_19; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memW_T_21 = _io_LSU_O_result_T_3 ? 64'h0 : _io_LSU_O_memW_T_20; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memW_T_22 = _io_LSU_O_result_T_1 ? 64'h0 : _io_LSU_O_memW_T_21; // @[Mux.scala 101:16]
  wire [1:0] _io_LSU_O_len_T_12 = _io_LSU_O_result_T_20 ? 2'h3 : 2'h0; // @[Mux.scala 101:16]
  wire [1:0] _io_LSU_O_len_T_13 = _io_LSU_O_result_T_19 ? 2'h2 : _io_LSU_O_len_T_12; // @[Mux.scala 101:16]
  wire [1:0] _io_LSU_O_len_T_14 = _io_LSU_O_result_T_18 ? 2'h1 : _io_LSU_O_len_T_13; // @[Mux.scala 101:16]
  wire [1:0] _io_LSU_O_len_T_15 = _io_LSU_O_result_T_17 ? 2'h0 : _io_LSU_O_len_T_14; // @[Mux.scala 101:16]
  wire [1:0] _io_LSU_O_len_T_16 = _io_LSU_O_result_T_16 ? 2'h3 : _io_LSU_O_len_T_15; // @[Mux.scala 101:16]
  wire [1:0] _io_LSU_O_len_T_17 = _io_LSU_O_result_T_13 ? 2'h2 : _io_LSU_O_len_T_16; // @[Mux.scala 101:16]
  wire [1:0] _io_LSU_O_len_T_18 = _io_LSU_O_result_T_11 ? 2'h2 : _io_LSU_O_len_T_17; // @[Mux.scala 101:16]
  wire [1:0] _io_LSU_O_len_T_19 = _io_LSU_O_result_T_8 ? 2'h1 : _io_LSU_O_len_T_18; // @[Mux.scala 101:16]
  wire [1:0] _io_LSU_O_len_T_20 = _io_LSU_O_result_T_6 ? 2'h1 : _io_LSU_O_len_T_19; // @[Mux.scala 101:16]
  wire [1:0] _io_LSU_O_len_T_21 = _io_LSU_O_result_T_3 ? 2'h0 : _io_LSU_O_len_T_20; // @[Mux.scala 101:16]
  wire [1:0] _io_LSU_O_len_T_22 = _io_LSU_O_result_T_1 ? 2'h0 : _io_LSU_O_len_T_21; // @[Mux.scala 101:16]
  assign io_LSU_O_result = _io_LSU_O_result_T_32[63:0]; // @[npc.scala 527:21]
  assign io_LSU_O_memAddr = _io_LSU_O_result_T ? 64'h0 : _io_LSU_O_memAddr_T_22; // @[Mux.scala 101:16]
  assign io_LSU_O_accessMem = _io_LSU_O_result_T ? 1'h0 : _io_LSU_O_result_T_1 | (_io_LSU_O_result_T_3 | (
    _io_LSU_O_result_T_6 | (_io_LSU_O_result_T_8 | (_io_LSU_O_result_T_11 | (_io_LSU_O_result_T_13 | (
    _io_LSU_O_result_T_16 | (_io_LSU_O_result_T_17 | (_io_LSU_O_result_T_18 | (_io_LSU_O_result_T_19 |
    _io_LSU_O_result_T_20))))))))); // @[Mux.scala 101:16]
  assign io_LSU_O_memRW = _io_LSU_O_result_T ? 1'h0 : _io_LSU_O_memRW_T_22; // @[Mux.scala 101:16]
  assign io_LSU_O_memW = _io_LSU_O_result_T ? 64'h0 : _io_LSU_O_memW_T_22; // @[Mux.scala 101:16]
  assign io_LSU_O_len = _io_LSU_O_result_T ? 2'h0 : _io_LSU_O_len_T_22; // @[Mux.scala 101:16]
endmodule
module WBU(
  input  [63:0] io_WBU_I_EXUresult,
  input  [63:0] io_WBU_I_LSUresult,
  input         io_WBU_I_LSUenable,
  input  [4:0]  io_WBU_I_rd,
  output [4:0]  io_WBU_O_GPRidx,
  output [63:0] io_WBU_O_GPRWriteBack
);
  assign io_WBU_O_GPRidx = io_WBU_I_rd; // @[npc.scala 637:21]
  assign io_WBU_O_GPRWriteBack = io_WBU_I_LSUenable ? io_WBU_I_LSUresult : io_WBU_I_EXUresult; // @[npc.scala 638:33]
endmodule
module npc(
  input         clock,
  input         reset,
  input  [63:0] io_NPC_startPC,
  output [63:0] io_NPC_sendCurrentPC,
  output [63:0] io_NPC_sendNextPC,
  input  [31:0] io_NPC_getInst,
  output [63:0] io_NPC_LSU_O_memAddr,
  output        io_NPC_LSU_O_accessMem,
  output        io_NPC_LSU_O_memRW,
  input  [63:0] io_NPC_LSU_I_memR,
  output [63:0] io_NPC_LSU_O_memW,
  output [1:0]  io_NPC_LSU_O_len,
  output        io_NPC_GPRchanged,
  output        io_NPC_halt,
  output [63:0] io_NPC_GPR00,
  output [63:0] io_NPC_GPR01,
  output [63:0] io_NPC_GPR02,
  output [63:0] io_NPC_GPR03,
  output [63:0] io_NPC_GPR04,
  output [63:0] io_NPC_GPR05,
  output [63:0] io_NPC_GPR06,
  output [63:0] io_NPC_GPR07,
  output [63:0] io_NPC_GPR08,
  output [63:0] io_NPC_GPR09,
  output [63:0] io_NPC_GPR10,
  output [63:0] io_NPC_GPR11,
  output [63:0] io_NPC_GPR12,
  output [63:0] io_NPC_GPR13,
  output [63:0] io_NPC_GPR14,
  output [63:0] io_NPC_GPR15,
  output [63:0] io_NPC_GPR16,
  output [63:0] io_NPC_GPR17,
  output [63:0] io_NPC_GPR18,
  output [63:0] io_NPC_GPR19,
  output [63:0] io_NPC_GPR20,
  output [63:0] io_NPC_GPR21,
  output [63:0] io_NPC_GPR22,
  output [63:0] io_NPC_GPR23,
  output [63:0] io_NPC_GPR24,
  output [63:0] io_NPC_GPR25,
  output [63:0] io_NPC_GPR26,
  output [63:0] io_NPC_GPR27,
  output [63:0] io_NPC_GPR28,
  output [63:0] io_NPC_GPR29,
  output [63:0] io_NPC_GPR30,
  output [63:0] io_NPC_GPR31
);
`ifdef RANDOMIZE_MEM_INIT
  reg [63:0] _RAND_0;
`endif // RANDOMIZE_MEM_INIT
`ifdef RANDOMIZE_REG_INIT
  reg [63:0] _RAND_1;
`endif // RANDOMIZE_REG_INIT
  reg [63:0] GPR [0:31]; // @[npc.scala 696:18]
  wire  GPR_npcb_src1_MPORT_en; // @[npc.scala 696:18]
  wire [4:0] GPR_npcb_src1_MPORT_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_npcb_src1_MPORT_data; // @[npc.scala 696:18]
  wire  GPR_npcb_src2_MPORT_en; // @[npc.scala 696:18]
  wire [4:0] GPR_npcb_src2_MPORT_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_npcb_src2_MPORT_data; // @[npc.scala 696:18]
  wire  GPR_MPORT_1_en; // @[npc.scala 696:18]
  wire [4:0] GPR_MPORT_1_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_MPORT_1_data; // @[npc.scala 696:18]
  wire  GPR_io_NPC_GPR00_MPORT_en; // @[npc.scala 696:18]
  wire [4:0] GPR_io_NPC_GPR00_MPORT_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_io_NPC_GPR00_MPORT_data; // @[npc.scala 696:18]
  wire  GPR_io_NPC_GPR01_MPORT_en; // @[npc.scala 696:18]
  wire [4:0] GPR_io_NPC_GPR01_MPORT_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_io_NPC_GPR01_MPORT_data; // @[npc.scala 696:18]
  wire  GPR_io_NPC_GPR02_MPORT_en; // @[npc.scala 696:18]
  wire [4:0] GPR_io_NPC_GPR02_MPORT_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_io_NPC_GPR02_MPORT_data; // @[npc.scala 696:18]
  wire  GPR_io_NPC_GPR03_MPORT_en; // @[npc.scala 696:18]
  wire [4:0] GPR_io_NPC_GPR03_MPORT_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_io_NPC_GPR03_MPORT_data; // @[npc.scala 696:18]
  wire  GPR_io_NPC_GPR04_MPORT_en; // @[npc.scala 696:18]
  wire [4:0] GPR_io_NPC_GPR04_MPORT_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_io_NPC_GPR04_MPORT_data; // @[npc.scala 696:18]
  wire  GPR_io_NPC_GPR05_MPORT_en; // @[npc.scala 696:18]
  wire [4:0] GPR_io_NPC_GPR05_MPORT_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_io_NPC_GPR05_MPORT_data; // @[npc.scala 696:18]
  wire  GPR_io_NPC_GPR06_MPORT_en; // @[npc.scala 696:18]
  wire [4:0] GPR_io_NPC_GPR06_MPORT_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_io_NPC_GPR06_MPORT_data; // @[npc.scala 696:18]
  wire  GPR_io_NPC_GPR07_MPORT_en; // @[npc.scala 696:18]
  wire [4:0] GPR_io_NPC_GPR07_MPORT_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_io_NPC_GPR07_MPORT_data; // @[npc.scala 696:18]
  wire  GPR_io_NPC_GPR08_MPORT_en; // @[npc.scala 696:18]
  wire [4:0] GPR_io_NPC_GPR08_MPORT_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_io_NPC_GPR08_MPORT_data; // @[npc.scala 696:18]
  wire  GPR_io_NPC_GPR09_MPORT_en; // @[npc.scala 696:18]
  wire [4:0] GPR_io_NPC_GPR09_MPORT_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_io_NPC_GPR09_MPORT_data; // @[npc.scala 696:18]
  wire  GPR_io_NPC_GPR10_MPORT_en; // @[npc.scala 696:18]
  wire [4:0] GPR_io_NPC_GPR10_MPORT_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_io_NPC_GPR10_MPORT_data; // @[npc.scala 696:18]
  wire  GPR_io_NPC_GPR11_MPORT_en; // @[npc.scala 696:18]
  wire [4:0] GPR_io_NPC_GPR11_MPORT_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_io_NPC_GPR11_MPORT_data; // @[npc.scala 696:18]
  wire  GPR_io_NPC_GPR12_MPORT_en; // @[npc.scala 696:18]
  wire [4:0] GPR_io_NPC_GPR12_MPORT_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_io_NPC_GPR12_MPORT_data; // @[npc.scala 696:18]
  wire  GPR_io_NPC_GPR13_MPORT_en; // @[npc.scala 696:18]
  wire [4:0] GPR_io_NPC_GPR13_MPORT_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_io_NPC_GPR13_MPORT_data; // @[npc.scala 696:18]
  wire  GPR_io_NPC_GPR14_MPORT_en; // @[npc.scala 696:18]
  wire [4:0] GPR_io_NPC_GPR14_MPORT_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_io_NPC_GPR14_MPORT_data; // @[npc.scala 696:18]
  wire  GPR_io_NPC_GPR15_MPORT_en; // @[npc.scala 696:18]
  wire [4:0] GPR_io_NPC_GPR15_MPORT_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_io_NPC_GPR15_MPORT_data; // @[npc.scala 696:18]
  wire  GPR_io_NPC_GPR16_MPORT_en; // @[npc.scala 696:18]
  wire [4:0] GPR_io_NPC_GPR16_MPORT_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_io_NPC_GPR16_MPORT_data; // @[npc.scala 696:18]
  wire  GPR_io_NPC_GPR17_MPORT_en; // @[npc.scala 696:18]
  wire [4:0] GPR_io_NPC_GPR17_MPORT_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_io_NPC_GPR17_MPORT_data; // @[npc.scala 696:18]
  wire  GPR_io_NPC_GPR18_MPORT_en; // @[npc.scala 696:18]
  wire [4:0] GPR_io_NPC_GPR18_MPORT_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_io_NPC_GPR18_MPORT_data; // @[npc.scala 696:18]
  wire  GPR_io_NPC_GPR19_MPORT_en; // @[npc.scala 696:18]
  wire [4:0] GPR_io_NPC_GPR19_MPORT_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_io_NPC_GPR19_MPORT_data; // @[npc.scala 696:18]
  wire  GPR_io_NPC_GPR20_MPORT_en; // @[npc.scala 696:18]
  wire [4:0] GPR_io_NPC_GPR20_MPORT_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_io_NPC_GPR20_MPORT_data; // @[npc.scala 696:18]
  wire  GPR_io_NPC_GPR21_MPORT_en; // @[npc.scala 696:18]
  wire [4:0] GPR_io_NPC_GPR21_MPORT_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_io_NPC_GPR21_MPORT_data; // @[npc.scala 696:18]
  wire  GPR_io_NPC_GPR22_MPORT_en; // @[npc.scala 696:18]
  wire [4:0] GPR_io_NPC_GPR22_MPORT_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_io_NPC_GPR22_MPORT_data; // @[npc.scala 696:18]
  wire  GPR_io_NPC_GPR23_MPORT_en; // @[npc.scala 696:18]
  wire [4:0] GPR_io_NPC_GPR23_MPORT_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_io_NPC_GPR23_MPORT_data; // @[npc.scala 696:18]
  wire  GPR_io_NPC_GPR24_MPORT_en; // @[npc.scala 696:18]
  wire [4:0] GPR_io_NPC_GPR24_MPORT_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_io_NPC_GPR24_MPORT_data; // @[npc.scala 696:18]
  wire  GPR_io_NPC_GPR25_MPORT_en; // @[npc.scala 696:18]
  wire [4:0] GPR_io_NPC_GPR25_MPORT_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_io_NPC_GPR25_MPORT_data; // @[npc.scala 696:18]
  wire  GPR_io_NPC_GPR26_MPORT_en; // @[npc.scala 696:18]
  wire [4:0] GPR_io_NPC_GPR26_MPORT_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_io_NPC_GPR26_MPORT_data; // @[npc.scala 696:18]
  wire  GPR_io_NPC_GPR27_MPORT_en; // @[npc.scala 696:18]
  wire [4:0] GPR_io_NPC_GPR27_MPORT_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_io_NPC_GPR27_MPORT_data; // @[npc.scala 696:18]
  wire  GPR_io_NPC_GPR28_MPORT_en; // @[npc.scala 696:18]
  wire [4:0] GPR_io_NPC_GPR28_MPORT_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_io_NPC_GPR28_MPORT_data; // @[npc.scala 696:18]
  wire  GPR_io_NPC_GPR29_MPORT_en; // @[npc.scala 696:18]
  wire [4:0] GPR_io_NPC_GPR29_MPORT_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_io_NPC_GPR29_MPORT_data; // @[npc.scala 696:18]
  wire  GPR_io_NPC_GPR30_MPORT_en; // @[npc.scala 696:18]
  wire [4:0] GPR_io_NPC_GPR30_MPORT_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_io_NPC_GPR30_MPORT_data; // @[npc.scala 696:18]
  wire  GPR_io_NPC_GPR31_MPORT_en; // @[npc.scala 696:18]
  wire [4:0] GPR_io_NPC_GPR31_MPORT_addr; // @[npc.scala 696:18]
  wire [63:0] GPR_io_NPC_GPR31_MPORT_data; // @[npc.scala 696:18]
  wire [63:0] GPR_MPORT_data; // @[npc.scala 696:18]
  wire [4:0] GPR_MPORT_addr; // @[npc.scala 696:18]
  wire  GPR_MPORT_mask; // @[npc.scala 696:18]
  wire  GPR_MPORT_en; // @[npc.scala 696:18]
  wire [63:0] GPR_MPORT_2_data; // @[npc.scala 696:18]
  wire [4:0] GPR_MPORT_2_addr; // @[npc.scala 696:18]
  wire  GPR_MPORT_2_mask; // @[npc.scala 696:18]
  wire  GPR_MPORT_2_en; // @[npc.scala 696:18]
  wire [31:0] npcb_IFU_io_IFU_I_inst; // @[npc.scala 700:26]
  wire [31:0] npcb_IDU_io_IDU_I_inst; // @[npc.scala 706:26]
  wire [4:0] npcb_IDU_io_IDU_O_rs1; // @[npc.scala 706:26]
  wire [4:0] npcb_IDU_io_IDU_O_rs2; // @[npc.scala 706:26]
  wire [4:0] npcb_IDU_io_IDU_O_rd; // @[npc.scala 706:26]
  wire [5:0] npcb_IDU_io_IDU_O_EXUopcode; // @[npc.scala 706:26]
  wire [5:0] npcb_IDU_io_IDU_O_LSUopcode; // @[npc.scala 706:26]
  wire  npcb_IDU_io_IDU_O_GPRneedWriteBack; // @[npc.scala 706:26]
  wire [63:0] npcb_IDU_io_IDU_O_imm; // @[npc.scala 706:26]
  wire  npcb_IDU_io_IDU_O_halt; // @[npc.scala 706:26]
  wire [63:0] npcb_EXU_io_EXU_I_src1; // @[npc.scala 715:26]
  wire [63:0] npcb_EXU_io_EXU_I_src2; // @[npc.scala 715:26]
  wire [63:0] npcb_EXU_io_EXU_I_imm; // @[npc.scala 715:26]
  wire [5:0] npcb_EXU_io_EXU_I_opcode; // @[npc.scala 715:26]
  wire [63:0] npcb_EXU_io_EXU_I_currentPC; // @[npc.scala 715:26]
  wire [63:0] npcb_EXU_io_EXU_O_result; // @[npc.scala 715:26]
  wire [63:0] npcb_EXU_io_EXU_O_staticNPC; // @[npc.scala 715:26]
  wire [63:0] npcb_EXU_io_EXU_O_dynamicNPC; // @[npc.scala 715:26]
  wire [63:0] npcb_LSU_io_LSU_I_src1; // @[npc.scala 723:26]
  wire [63:0] npcb_LSU_io_LSU_I_src2; // @[npc.scala 723:26]
  wire [5:0] npcb_LSU_io_LSU_I_opcode; // @[npc.scala 723:26]
  wire [63:0] npcb_LSU_io_LSU_O_result; // @[npc.scala 723:26]
  wire [63:0] npcb_LSU_io_LSU_O_memAddr; // @[npc.scala 723:26]
  wire  npcb_LSU_io_LSU_O_accessMem; // @[npc.scala 723:26]
  wire  npcb_LSU_io_LSU_O_memRW; // @[npc.scala 723:26]
  wire [63:0] npcb_LSU_io_LSU_I_memR; // @[npc.scala 723:26]
  wire [63:0] npcb_LSU_io_LSU_O_memW; // @[npc.scala 723:26]
  wire [1:0] npcb_LSU_io_LSU_O_len; // @[npc.scala 723:26]
  wire [63:0] npcb_WBU_io_WBU_I_EXUresult; // @[npc.scala 736:26]
  wire [63:0] npcb_WBU_io_WBU_I_LSUresult; // @[npc.scala 736:26]
  wire  npcb_WBU_io_WBU_I_LSUenable; // @[npc.scala 736:26]
  wire [4:0] npcb_WBU_io_WBU_I_rd; // @[npc.scala 736:26]
  wire [4:0] npcb_WBU_io_WBU_O_GPRidx; // @[npc.scala 736:26]
  wire [63:0] npcb_WBU_io_WBU_O_GPRWriteBack; // @[npc.scala 736:26]
  reg [63:0] PC; // @[npc.scala 694:21]
  wire [63:0] _T_1 = npcb_WBU_io_WBU_O_GPRidx == 5'h0 ? 64'h0 : GPR_MPORT_1_data; // @[npc.scala 697:37]
  IFU npcb_IFU ( // @[npc.scala 700:26]
    .io_IFU_I_inst(npcb_IFU_io_IFU_I_inst)
  );
  IDU npcb_IDU ( // @[npc.scala 706:26]
    .io_IDU_I_inst(npcb_IDU_io_IDU_I_inst),
    .io_IDU_O_rs1(npcb_IDU_io_IDU_O_rs1),
    .io_IDU_O_rs2(npcb_IDU_io_IDU_O_rs2),
    .io_IDU_O_rd(npcb_IDU_io_IDU_O_rd),
    .io_IDU_O_EXUopcode(npcb_IDU_io_IDU_O_EXUopcode),
    .io_IDU_O_LSUopcode(npcb_IDU_io_IDU_O_LSUopcode),
    .io_IDU_O_GPRneedWriteBack(npcb_IDU_io_IDU_O_GPRneedWriteBack),
    .io_IDU_O_imm(npcb_IDU_io_IDU_O_imm),
    .io_IDU_O_halt(npcb_IDU_io_IDU_O_halt)
  );
  EXU npcb_EXU ( // @[npc.scala 715:26]
    .io_EXU_I_src1(npcb_EXU_io_EXU_I_src1),
    .io_EXU_I_src2(npcb_EXU_io_EXU_I_src2),
    .io_EXU_I_imm(npcb_EXU_io_EXU_I_imm),
    .io_EXU_I_opcode(npcb_EXU_io_EXU_I_opcode),
    .io_EXU_I_currentPC(npcb_EXU_io_EXU_I_currentPC),
    .io_EXU_O_result(npcb_EXU_io_EXU_O_result),
    .io_EXU_O_staticNPC(npcb_EXU_io_EXU_O_staticNPC),
    .io_EXU_O_dynamicNPC(npcb_EXU_io_EXU_O_dynamicNPC)
  );
  LSU npcb_LSU ( // @[npc.scala 723:26]
    .io_LSU_I_src1(npcb_LSU_io_LSU_I_src1),
    .io_LSU_I_src2(npcb_LSU_io_LSU_I_src2),
    .io_LSU_I_opcode(npcb_LSU_io_LSU_I_opcode),
    .io_LSU_O_result(npcb_LSU_io_LSU_O_result),
    .io_LSU_O_memAddr(npcb_LSU_io_LSU_O_memAddr),
    .io_LSU_O_accessMem(npcb_LSU_io_LSU_O_accessMem),
    .io_LSU_O_memRW(npcb_LSU_io_LSU_O_memRW),
    .io_LSU_I_memR(npcb_LSU_io_LSU_I_memR),
    .io_LSU_O_memW(npcb_LSU_io_LSU_O_memW),
    .io_LSU_O_len(npcb_LSU_io_LSU_O_len)
  );
  WBU npcb_WBU ( // @[npc.scala 736:26]
    .io_WBU_I_EXUresult(npcb_WBU_io_WBU_I_EXUresult),
    .io_WBU_I_LSUresult(npcb_WBU_io_WBU_I_LSUresult),
    .io_WBU_I_LSUenable(npcb_WBU_io_WBU_I_LSUenable),
    .io_WBU_I_rd(npcb_WBU_io_WBU_I_rd),
    .io_WBU_O_GPRidx(npcb_WBU_io_WBU_O_GPRidx),
    .io_WBU_O_GPRWriteBack(npcb_WBU_io_WBU_O_GPRWriteBack)
  );
  assign GPR_npcb_src1_MPORT_en = 1'h1;
  assign GPR_npcb_src1_MPORT_addr = npcb_IDU_io_IDU_O_rs1;
  assign GPR_npcb_src1_MPORT_data = GPR[GPR_npcb_src1_MPORT_addr]; // @[npc.scala 696:18]
  assign GPR_npcb_src2_MPORT_en = 1'h1;
  assign GPR_npcb_src2_MPORT_addr = npcb_IDU_io_IDU_O_rs2;
  assign GPR_npcb_src2_MPORT_data = GPR[GPR_npcb_src2_MPORT_addr]; // @[npc.scala 696:18]
  assign GPR_MPORT_1_en = 1'h1;
  assign GPR_MPORT_1_addr = npcb_WBU_io_WBU_O_GPRidx;
  assign GPR_MPORT_1_data = GPR[GPR_MPORT_1_addr]; // @[npc.scala 696:18]
  assign GPR_io_NPC_GPR00_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR00_MPORT_addr = 5'h0;
  assign GPR_io_NPC_GPR00_MPORT_data = GPR[GPR_io_NPC_GPR00_MPORT_addr]; // @[npc.scala 696:18]
  assign GPR_io_NPC_GPR01_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR01_MPORT_addr = 5'h1;
  assign GPR_io_NPC_GPR01_MPORT_data = GPR[GPR_io_NPC_GPR01_MPORT_addr]; // @[npc.scala 696:18]
  assign GPR_io_NPC_GPR02_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR02_MPORT_addr = 5'h2;
  assign GPR_io_NPC_GPR02_MPORT_data = GPR[GPR_io_NPC_GPR02_MPORT_addr]; // @[npc.scala 696:18]
  assign GPR_io_NPC_GPR03_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR03_MPORT_addr = 5'h3;
  assign GPR_io_NPC_GPR03_MPORT_data = GPR[GPR_io_NPC_GPR03_MPORT_addr]; // @[npc.scala 696:18]
  assign GPR_io_NPC_GPR04_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR04_MPORT_addr = 5'h4;
  assign GPR_io_NPC_GPR04_MPORT_data = GPR[GPR_io_NPC_GPR04_MPORT_addr]; // @[npc.scala 696:18]
  assign GPR_io_NPC_GPR05_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR05_MPORT_addr = 5'h5;
  assign GPR_io_NPC_GPR05_MPORT_data = GPR[GPR_io_NPC_GPR05_MPORT_addr]; // @[npc.scala 696:18]
  assign GPR_io_NPC_GPR06_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR06_MPORT_addr = 5'h6;
  assign GPR_io_NPC_GPR06_MPORT_data = GPR[GPR_io_NPC_GPR06_MPORT_addr]; // @[npc.scala 696:18]
  assign GPR_io_NPC_GPR07_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR07_MPORT_addr = 5'h7;
  assign GPR_io_NPC_GPR07_MPORT_data = GPR[GPR_io_NPC_GPR07_MPORT_addr]; // @[npc.scala 696:18]
  assign GPR_io_NPC_GPR08_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR08_MPORT_addr = 5'h8;
  assign GPR_io_NPC_GPR08_MPORT_data = GPR[GPR_io_NPC_GPR08_MPORT_addr]; // @[npc.scala 696:18]
  assign GPR_io_NPC_GPR09_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR09_MPORT_addr = 5'h9;
  assign GPR_io_NPC_GPR09_MPORT_data = GPR[GPR_io_NPC_GPR09_MPORT_addr]; // @[npc.scala 696:18]
  assign GPR_io_NPC_GPR10_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR10_MPORT_addr = 5'ha;
  assign GPR_io_NPC_GPR10_MPORT_data = GPR[GPR_io_NPC_GPR10_MPORT_addr]; // @[npc.scala 696:18]
  assign GPR_io_NPC_GPR11_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR11_MPORT_addr = 5'hb;
  assign GPR_io_NPC_GPR11_MPORT_data = GPR[GPR_io_NPC_GPR11_MPORT_addr]; // @[npc.scala 696:18]
  assign GPR_io_NPC_GPR12_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR12_MPORT_addr = 5'hc;
  assign GPR_io_NPC_GPR12_MPORT_data = GPR[GPR_io_NPC_GPR12_MPORT_addr]; // @[npc.scala 696:18]
  assign GPR_io_NPC_GPR13_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR13_MPORT_addr = 5'hd;
  assign GPR_io_NPC_GPR13_MPORT_data = GPR[GPR_io_NPC_GPR13_MPORT_addr]; // @[npc.scala 696:18]
  assign GPR_io_NPC_GPR14_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR14_MPORT_addr = 5'he;
  assign GPR_io_NPC_GPR14_MPORT_data = GPR[GPR_io_NPC_GPR14_MPORT_addr]; // @[npc.scala 696:18]
  assign GPR_io_NPC_GPR15_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR15_MPORT_addr = 5'hf;
  assign GPR_io_NPC_GPR15_MPORT_data = GPR[GPR_io_NPC_GPR15_MPORT_addr]; // @[npc.scala 696:18]
  assign GPR_io_NPC_GPR16_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR16_MPORT_addr = 5'h10;
  assign GPR_io_NPC_GPR16_MPORT_data = GPR[GPR_io_NPC_GPR16_MPORT_addr]; // @[npc.scala 696:18]
  assign GPR_io_NPC_GPR17_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR17_MPORT_addr = 5'h11;
  assign GPR_io_NPC_GPR17_MPORT_data = GPR[GPR_io_NPC_GPR17_MPORT_addr]; // @[npc.scala 696:18]
  assign GPR_io_NPC_GPR18_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR18_MPORT_addr = 5'h12;
  assign GPR_io_NPC_GPR18_MPORT_data = GPR[GPR_io_NPC_GPR18_MPORT_addr]; // @[npc.scala 696:18]
  assign GPR_io_NPC_GPR19_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR19_MPORT_addr = 5'h13;
  assign GPR_io_NPC_GPR19_MPORT_data = GPR[GPR_io_NPC_GPR19_MPORT_addr]; // @[npc.scala 696:18]
  assign GPR_io_NPC_GPR20_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR20_MPORT_addr = 5'h14;
  assign GPR_io_NPC_GPR20_MPORT_data = GPR[GPR_io_NPC_GPR20_MPORT_addr]; // @[npc.scala 696:18]
  assign GPR_io_NPC_GPR21_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR21_MPORT_addr = 5'h15;
  assign GPR_io_NPC_GPR21_MPORT_data = GPR[GPR_io_NPC_GPR21_MPORT_addr]; // @[npc.scala 696:18]
  assign GPR_io_NPC_GPR22_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR22_MPORT_addr = 5'h16;
  assign GPR_io_NPC_GPR22_MPORT_data = GPR[GPR_io_NPC_GPR22_MPORT_addr]; // @[npc.scala 696:18]
  assign GPR_io_NPC_GPR23_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR23_MPORT_addr = 5'h17;
  assign GPR_io_NPC_GPR23_MPORT_data = GPR[GPR_io_NPC_GPR23_MPORT_addr]; // @[npc.scala 696:18]
  assign GPR_io_NPC_GPR24_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR24_MPORT_addr = 5'h18;
  assign GPR_io_NPC_GPR24_MPORT_data = GPR[GPR_io_NPC_GPR24_MPORT_addr]; // @[npc.scala 696:18]
  assign GPR_io_NPC_GPR25_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR25_MPORT_addr = 5'h19;
  assign GPR_io_NPC_GPR25_MPORT_data = GPR[GPR_io_NPC_GPR25_MPORT_addr]; // @[npc.scala 696:18]
  assign GPR_io_NPC_GPR26_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR26_MPORT_addr = 5'h1a;
  assign GPR_io_NPC_GPR26_MPORT_data = GPR[GPR_io_NPC_GPR26_MPORT_addr]; // @[npc.scala 696:18]
  assign GPR_io_NPC_GPR27_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR27_MPORT_addr = 5'h1b;
  assign GPR_io_NPC_GPR27_MPORT_data = GPR[GPR_io_NPC_GPR27_MPORT_addr]; // @[npc.scala 696:18]
  assign GPR_io_NPC_GPR28_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR28_MPORT_addr = 5'h1c;
  assign GPR_io_NPC_GPR28_MPORT_data = GPR[GPR_io_NPC_GPR28_MPORT_addr]; // @[npc.scala 696:18]
  assign GPR_io_NPC_GPR29_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR29_MPORT_addr = 5'h1d;
  assign GPR_io_NPC_GPR29_MPORT_data = GPR[GPR_io_NPC_GPR29_MPORT_addr]; // @[npc.scala 696:18]
  assign GPR_io_NPC_GPR30_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR30_MPORT_addr = 5'h1e;
  assign GPR_io_NPC_GPR30_MPORT_data = GPR[GPR_io_NPC_GPR30_MPORT_addr]; // @[npc.scala 696:18]
  assign GPR_io_NPC_GPR31_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR31_MPORT_addr = 5'h1f;
  assign GPR_io_NPC_GPR31_MPORT_data = GPR[GPR_io_NPC_GPR31_MPORT_addr]; // @[npc.scala 696:18]
  assign GPR_MPORT_data = npcb_IDU_io_IDU_O_GPRneedWriteBack ? npcb_WBU_io_WBU_O_GPRWriteBack : _T_1;
  assign GPR_MPORT_addr = npcb_WBU_io_WBU_O_GPRidx;
  assign GPR_MPORT_mask = 1'h1;
  assign GPR_MPORT_en = 1'h1;
  assign GPR_MPORT_2_data = 64'h0;
  assign GPR_MPORT_2_addr = 5'h0;
  assign GPR_MPORT_2_mask = 1'h1;
  assign GPR_MPORT_2_en = 1'h1;
  assign io_NPC_sendCurrentPC = PC; // @[npc.scala 703:26]
  assign io_NPC_sendNextPC = PC; // @[npc.scala 745:23]
  assign io_NPC_LSU_O_memAddr = npcb_LSU_io_LSU_O_memAddr; // @[npc.scala 731:26]
  assign io_NPC_LSU_O_accessMem = npcb_LSU_io_LSU_O_accessMem; // @[npc.scala 729:28]
  assign io_NPC_LSU_O_memRW = npcb_LSU_io_LSU_O_memRW; // @[npc.scala 730:24]
  assign io_NPC_LSU_O_memW = npcb_LSU_io_LSU_O_memW; // @[npc.scala 732:23]
  assign io_NPC_LSU_O_len = npcb_LSU_io_LSU_O_len; // @[npc.scala 728:22]
  assign io_NPC_GPRchanged = npcb_IDU_io_IDU_O_GPRneedWriteBack; // @[npc.scala 741:23]
  assign io_NPC_halt = npcb_IDU_io_IDU_O_halt; // @[npc.scala 712:17]
  assign io_NPC_GPR00 = 64'h0; // @[npc.scala 697:37]
  assign io_NPC_GPR01 = GPR_io_NPC_GPR01_MPORT_data; // @[npc.scala 697:37]
  assign io_NPC_GPR02 = GPR_io_NPC_GPR02_MPORT_data; // @[npc.scala 697:37]
  assign io_NPC_GPR03 = GPR_io_NPC_GPR03_MPORT_data; // @[npc.scala 697:37]
  assign io_NPC_GPR04 = GPR_io_NPC_GPR04_MPORT_data; // @[npc.scala 697:37]
  assign io_NPC_GPR05 = GPR_io_NPC_GPR05_MPORT_data; // @[npc.scala 697:37]
  assign io_NPC_GPR06 = GPR_io_NPC_GPR06_MPORT_data; // @[npc.scala 697:37]
  assign io_NPC_GPR07 = GPR_io_NPC_GPR07_MPORT_data; // @[npc.scala 697:37]
  assign io_NPC_GPR08 = GPR_io_NPC_GPR08_MPORT_data; // @[npc.scala 697:37]
  assign io_NPC_GPR09 = GPR_io_NPC_GPR09_MPORT_data; // @[npc.scala 697:37]
  assign io_NPC_GPR10 = GPR_io_NPC_GPR10_MPORT_data; // @[npc.scala 697:37]
  assign io_NPC_GPR11 = GPR_io_NPC_GPR11_MPORT_data; // @[npc.scala 697:37]
  assign io_NPC_GPR12 = GPR_io_NPC_GPR12_MPORT_data; // @[npc.scala 697:37]
  assign io_NPC_GPR13 = GPR_io_NPC_GPR13_MPORT_data; // @[npc.scala 697:37]
  assign io_NPC_GPR14 = GPR_io_NPC_GPR14_MPORT_data; // @[npc.scala 697:37]
  assign io_NPC_GPR15 = GPR_io_NPC_GPR15_MPORT_data; // @[npc.scala 697:37]
  assign io_NPC_GPR16 = GPR_io_NPC_GPR16_MPORT_data; // @[npc.scala 697:37]
  assign io_NPC_GPR17 = GPR_io_NPC_GPR17_MPORT_data; // @[npc.scala 697:37]
  assign io_NPC_GPR18 = GPR_io_NPC_GPR18_MPORT_data; // @[npc.scala 697:37]
  assign io_NPC_GPR19 = GPR_io_NPC_GPR19_MPORT_data; // @[npc.scala 697:37]
  assign io_NPC_GPR20 = GPR_io_NPC_GPR20_MPORT_data; // @[npc.scala 697:37]
  assign io_NPC_GPR21 = GPR_io_NPC_GPR21_MPORT_data; // @[npc.scala 697:37]
  assign io_NPC_GPR22 = GPR_io_NPC_GPR22_MPORT_data; // @[npc.scala 697:37]
  assign io_NPC_GPR23 = GPR_io_NPC_GPR23_MPORT_data; // @[npc.scala 697:37]
  assign io_NPC_GPR24 = GPR_io_NPC_GPR24_MPORT_data; // @[npc.scala 697:37]
  assign io_NPC_GPR25 = GPR_io_NPC_GPR25_MPORT_data; // @[npc.scala 697:37]
  assign io_NPC_GPR26 = GPR_io_NPC_GPR26_MPORT_data; // @[npc.scala 697:37]
  assign io_NPC_GPR27 = GPR_io_NPC_GPR27_MPORT_data; // @[npc.scala 697:37]
  assign io_NPC_GPR28 = GPR_io_NPC_GPR28_MPORT_data; // @[npc.scala 697:37]
  assign io_NPC_GPR29 = GPR_io_NPC_GPR29_MPORT_data; // @[npc.scala 697:37]
  assign io_NPC_GPR30 = GPR_io_NPC_GPR30_MPORT_data; // @[npc.scala 697:37]
  assign io_NPC_GPR31 = GPR_io_NPC_GPR31_MPORT_data; // @[npc.scala 697:37]
  assign npcb_IFU_io_IFU_I_inst = io_NPC_getInst; // @[npc.scala 701:28]
  assign npcb_IDU_io_IDU_I_inst = npcb_IFU_io_IFU_I_inst; // @[npc.scala 707:28]
  assign npcb_EXU_io_EXU_I_src1 = npcb_IDU_io_IDU_O_rs1 == 5'h0 ? 64'h0 : GPR_npcb_src1_MPORT_data; // @[npc.scala 697:37]
  assign npcb_EXU_io_EXU_I_src2 = npcb_IDU_io_IDU_O_rs2 == 5'h0 ? 64'h0 : GPR_npcb_src2_MPORT_data; // @[npc.scala 697:37]
  assign npcb_EXU_io_EXU_I_imm = npcb_IDU_io_IDU_O_imm; // @[npc.scala 718:27]
  assign npcb_EXU_io_EXU_I_opcode = npcb_IDU_io_IDU_O_EXUopcode; // @[npc.scala 719:30]
  assign npcb_EXU_io_EXU_I_currentPC = PC; // @[npc.scala 720:33]
  assign npcb_LSU_io_LSU_I_src1 = npcb_EXU_io_EXU_O_result; // @[npc.scala 724:28]
  assign npcb_LSU_io_LSU_I_src2 = npcb_IDU_io_IDU_O_rs2 == 5'h0 ? 64'h0 : GPR_npcb_src2_MPORT_data; // @[npc.scala 697:37]
  assign npcb_LSU_io_LSU_I_opcode = npcb_IDU_io_IDU_O_LSUopcode; // @[npc.scala 727:30]
  assign npcb_LSU_io_LSU_I_memR = io_NPC_LSU_I_memR; // @[npc.scala 733:28]
  assign npcb_WBU_io_WBU_I_EXUresult = npcb_EXU_io_EXU_O_result; // @[npc.scala 738:33]
  assign npcb_WBU_io_WBU_I_LSUresult = npcb_LSU_io_LSU_O_result; // @[npc.scala 739:33]
  assign npcb_WBU_io_WBU_I_LSUenable = ~(npcb_IDU_io_IDU_O_LSUopcode == 6'h0); // @[npc.scala 737:36]
  assign npcb_WBU_io_WBU_I_rd = npcb_IDU_io_IDU_O_rd; // @[npc.scala 740:26]
  always @(posedge clock) begin
    if (GPR_MPORT_en & GPR_MPORT_mask) begin
      GPR[GPR_MPORT_addr] <= GPR_MPORT_data; // @[npc.scala 696:18]
    end
    if (GPR_MPORT_2_en & GPR_MPORT_2_mask) begin
      GPR[GPR_MPORT_2_addr] <= GPR_MPORT_2_data; // @[npc.scala 696:18]
    end
    if (reset) begin // @[npc.scala 694:21]
      PC <= io_NPC_startPC; // @[npc.scala 694:21]
    end else begin
      PC <= npcb_EXU_io_EXU_O_dynamicNPC; // @[npc.scala 744:8]
    end
  end
// Register and memory initialization
`ifdef RANDOMIZE_GARBAGE_ASSIGN
`define RANDOMIZE
`endif
`ifdef RANDOMIZE_INVALID_ASSIGN
`define RANDOMIZE
`endif
`ifdef RANDOMIZE_REG_INIT
`define RANDOMIZE
`endif
`ifdef RANDOMIZE_MEM_INIT
`define RANDOMIZE
`endif
`ifndef RANDOM
`define RANDOM $random
`endif
`ifdef RANDOMIZE_MEM_INIT
  integer initvar;
`endif
`ifndef SYNTHESIS
`ifdef FIRRTL_BEFORE_INITIAL
`FIRRTL_BEFORE_INITIAL
`endif
initial begin
  `ifdef RANDOMIZE
    `ifdef INIT_RANDOM
      `INIT_RANDOM
    `endif
    `ifndef VERILATOR
      `ifdef RANDOMIZE_DELAY
        #`RANDOMIZE_DELAY begin end
      `else
        #0.002 begin end
      `endif
    `endif
`ifdef RANDOMIZE_MEM_INIT
  _RAND_0 = {2{`RANDOM}};
  for (initvar = 0; initvar < 32; initvar = initvar+1)
    GPR[initvar] = _RAND_0[63:0];
`endif // RANDOMIZE_MEM_INIT
`ifdef RANDOMIZE_REG_INIT
  _RAND_1 = {2{`RANDOM}};
  PC = _RAND_1[63:0];
`endif // RANDOMIZE_REG_INIT
  `endif // RANDOMIZE
end // initial
`ifdef FIRRTL_AFTER_INITIAL
`FIRRTL_AFTER_INITIAL
`endif
`endif // SYNTHESIS
endmodule
/* verilator lint_on WIDTH */
