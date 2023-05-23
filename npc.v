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
  output        io_IDU_O_snpcISdnpc,
  output        io_IDU_O_GPRneedWriteBack,
  output [63:0] io_IDU_O_imm,
  output        io_IDU_O_halt,
  output        io_IDU_O_error
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
  wire  _IDU_opcodes_T_99 = 32'h4000503b == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_101 = 32'h2000033 == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_103 = 32'h2001033 == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_105 = 32'h2002033 == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_107 = 32'h2003033 == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_109 = 32'h2004033 == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_111 = 32'h2005033 == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_113 = 32'h2006033 == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_115 = 32'h2007033 == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_117 = 32'h200003b == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_119 = 32'h200403b == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_121 = 32'h200503b == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_123 = 32'h200603b == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire  _IDU_opcodes_T_125 = 32'h200703b == _IDU_opcodes_T_54; // @[Lookup.scala 31:38]
  wire [2:0] _IDU_opcodes_T_126 = _IDU_opcodes_T_125 ? 3'h0 : 3'h7; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_127 = _IDU_opcodes_T_123 ? 3'h0 : _IDU_opcodes_T_126; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_128 = _IDU_opcodes_T_121 ? 3'h0 : _IDU_opcodes_T_127; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_129 = _IDU_opcodes_T_119 ? 3'h0 : _IDU_opcodes_T_128; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_130 = _IDU_opcodes_T_117 ? 3'h0 : _IDU_opcodes_T_129; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_131 = _IDU_opcodes_T_115 ? 3'h0 : _IDU_opcodes_T_130; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_132 = _IDU_opcodes_T_113 ? 3'h0 : _IDU_opcodes_T_131; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_133 = _IDU_opcodes_T_111 ? 3'h0 : _IDU_opcodes_T_132; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_134 = _IDU_opcodes_T_109 ? 3'h0 : _IDU_opcodes_T_133; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_135 = _IDU_opcodes_T_107 ? 3'h0 : _IDU_opcodes_T_134; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_136 = _IDU_opcodes_T_105 ? 3'h0 : _IDU_opcodes_T_135; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_137 = _IDU_opcodes_T_103 ? 3'h0 : _IDU_opcodes_T_136; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_138 = _IDU_opcodes_T_101 ? 3'h0 : _IDU_opcodes_T_137; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_139 = _IDU_opcodes_T_99 ? 3'h0 : _IDU_opcodes_T_138; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_140 = _IDU_opcodes_T_97 ? 3'h0 : _IDU_opcodes_T_139; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_141 = _IDU_opcodes_T_95 ? 3'h0 : _IDU_opcodes_T_140; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_142 = _IDU_opcodes_T_83 ? 3'h0 : _IDU_opcodes_T_141; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_143 = _IDU_opcodes_T_91 ? 3'h0 : _IDU_opcodes_T_142; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_144 = _IDU_opcodes_T_89 ? 3'h0 : _IDU_opcodes_T_143; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_145 = _IDU_opcodes_T_87 ? 3'h0 : _IDU_opcodes_T_144; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_146 = _IDU_opcodes_T_85 ? 3'h0 : _IDU_opcodes_T_145; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_147 = _IDU_opcodes_T_83 ? 3'h1 : _IDU_opcodes_T_146; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_148 = _IDU_opcodes_T_81 ? 3'h2 : _IDU_opcodes_T_147; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_149 = _IDU_opcodes_T_79 ? 3'h1 : _IDU_opcodes_T_148; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_150 = _IDU_opcodes_T_77 ? 3'h1 : _IDU_opcodes_T_149; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_151 = _IDU_opcodes_T_75 ? 3'h6 : _IDU_opcodes_T_150; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_152 = _IDU_opcodes_T_73 ? 3'h0 : _IDU_opcodes_T_151; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_153 = _IDU_opcodes_T_71 ? 3'h0 : _IDU_opcodes_T_152; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_154 = _IDU_opcodes_T_69 ? 3'h0 : _IDU_opcodes_T_153; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_155 = _IDU_opcodes_T_67 ? 3'h0 : _IDU_opcodes_T_154; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_156 = _IDU_opcodes_T_65 ? 3'h0 : _IDU_opcodes_T_155; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_157 = _IDU_opcodes_T_63 ? 3'h0 : _IDU_opcodes_T_156; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_158 = _IDU_opcodes_T_61 ? 3'h0 : _IDU_opcodes_T_157; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_159 = _IDU_opcodes_T_59 ? 3'h0 : _IDU_opcodes_T_158; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_160 = _IDU_opcodes_T_57 ? 3'h0 : _IDU_opcodes_T_159; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_161 = _IDU_opcodes_T_55 ? 3'h0 : _IDU_opcodes_T_160; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_162 = _IDU_opcodes_T_53 ? 3'h1 : _IDU_opcodes_T_161; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_163 = _IDU_opcodes_T_51 ? 3'h1 : _IDU_opcodes_T_162; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_164 = _IDU_opcodes_T_49 ? 3'h1 : _IDU_opcodes_T_163; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_165 = _IDU_opcodes_T_47 ? 3'h1 : _IDU_opcodes_T_164; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_166 = _IDU_opcodes_T_45 ? 3'h1 : _IDU_opcodes_T_165; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_167 = _IDU_opcodes_T_43 ? 3'h1 : _IDU_opcodes_T_166; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_168 = _IDU_opcodes_T_41 ? 3'h1 : _IDU_opcodes_T_167; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_169 = _IDU_opcodes_T_39 ? 3'h1 : _IDU_opcodes_T_168; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_170 = _IDU_opcodes_T_37 ? 3'h1 : _IDU_opcodes_T_169; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_171 = _IDU_opcodes_T_35 ? 3'h2 : _IDU_opcodes_T_170; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_172 = _IDU_opcodes_T_33 ? 3'h2 : _IDU_opcodes_T_171; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_173 = _IDU_opcodes_T_31 ? 3'h2 : _IDU_opcodes_T_172; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_174 = _IDU_opcodes_T_29 ? 3'h1 : _IDU_opcodes_T_173; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_175 = _IDU_opcodes_T_27 ? 3'h1 : _IDU_opcodes_T_174; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_176 = _IDU_opcodes_T_25 ? 3'h1 : _IDU_opcodes_T_175; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_177 = _IDU_opcodes_T_23 ? 3'h1 : _IDU_opcodes_T_176; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_178 = _IDU_opcodes_T_21 ? 3'h1 : _IDU_opcodes_T_177; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_179 = _IDU_opcodes_T_19 ? 3'h3 : _IDU_opcodes_T_178; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_180 = _IDU_opcodes_T_17 ? 3'h3 : _IDU_opcodes_T_179; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_181 = _IDU_opcodes_T_15 ? 3'h3 : _IDU_opcodes_T_180; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_182 = _IDU_opcodes_T_13 ? 3'h3 : _IDU_opcodes_T_181; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_183 = _IDU_opcodes_T_11 ? 3'h3 : _IDU_opcodes_T_182; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_184 = _IDU_opcodes_T_9 ? 3'h3 : _IDU_opcodes_T_183; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_185 = _IDU_opcodes_T_7 ? 3'h1 : _IDU_opcodes_T_184; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_186 = _IDU_opcodes_T_5 ? 3'h5 : _IDU_opcodes_T_185; // @[Lookup.scala 34:39]
  wire [2:0] _IDU_opcodes_T_187 = _IDU_opcodes_T_3 ? 3'h4 : _IDU_opcodes_T_186; // @[Lookup.scala 34:39]
  wire [2:0] IDU_inst_type = _IDU_opcodes_T_1 ? 3'h4 : _IDU_opcodes_T_187; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_188 = _IDU_opcodes_T_125 ? 6'h3d : 6'h0; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_189 = _IDU_opcodes_T_123 ? 6'h3c : _IDU_opcodes_T_188; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_190 = _IDU_opcodes_T_121 ? 6'h3b : _IDU_opcodes_T_189; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_191 = _IDU_opcodes_T_119 ? 6'h3a : _IDU_opcodes_T_190; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_192 = _IDU_opcodes_T_117 ? 6'h39 : _IDU_opcodes_T_191; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_193 = _IDU_opcodes_T_115 ? 6'h38 : _IDU_opcodes_T_192; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_194 = _IDU_opcodes_T_113 ? 6'h37 : _IDU_opcodes_T_193; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_195 = _IDU_opcodes_T_111 ? 6'h36 : _IDU_opcodes_T_194; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_196 = _IDU_opcodes_T_109 ? 6'h35 : _IDU_opcodes_T_195; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_197 = _IDU_opcodes_T_107 ? 6'h34 : _IDU_opcodes_T_196; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_198 = _IDU_opcodes_T_105 ? 6'h33 : _IDU_opcodes_T_197; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_199 = _IDU_opcodes_T_103 ? 6'h32 : _IDU_opcodes_T_198; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_200 = _IDU_opcodes_T_101 ? 6'h31 : _IDU_opcodes_T_199; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_201 = _IDU_opcodes_T_99 ? 6'h30 : _IDU_opcodes_T_200; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_202 = _IDU_opcodes_T_97 ? 6'h2e : _IDU_opcodes_T_201; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_203 = _IDU_opcodes_T_95 ? 6'h2d : _IDU_opcodes_T_202; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_204 = _IDU_opcodes_T_83 ? 6'h28 : _IDU_opcodes_T_203; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_205 = _IDU_opcodes_T_91 ? 6'h2c : _IDU_opcodes_T_204; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_206 = _IDU_opcodes_T_89 ? 6'h2b : _IDU_opcodes_T_205; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_207 = _IDU_opcodes_T_87 ? 6'h2a : _IDU_opcodes_T_206; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_208 = _IDU_opcodes_T_85 ? 6'h29 : _IDU_opcodes_T_207; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_209 = _IDU_opcodes_T_83 ? 6'h28 : _IDU_opcodes_T_208; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_210 = _IDU_opcodes_T_81 ? 6'h27 : _IDU_opcodes_T_209; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_211 = _IDU_opcodes_T_79 ? 6'h26 : _IDU_opcodes_T_210; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_212 = _IDU_opcodes_T_77 ? 6'h25 : _IDU_opcodes_T_211; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_213 = _IDU_opcodes_T_75 ? 6'h0 : _IDU_opcodes_T_212; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_214 = _IDU_opcodes_T_73 ? 6'h24 : _IDU_opcodes_T_213; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_215 = _IDU_opcodes_T_71 ? 6'h23 : _IDU_opcodes_T_214; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_216 = _IDU_opcodes_T_69 ? 6'h22 : _IDU_opcodes_T_215; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_217 = _IDU_opcodes_T_67 ? 6'h21 : _IDU_opcodes_T_216; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_218 = _IDU_opcodes_T_65 ? 6'h20 : _IDU_opcodes_T_217; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_219 = _IDU_opcodes_T_63 ? 6'h1f : _IDU_opcodes_T_218; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_220 = _IDU_opcodes_T_61 ? 6'h1e : _IDU_opcodes_T_219; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_221 = _IDU_opcodes_T_59 ? 6'h1d : _IDU_opcodes_T_220; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_222 = _IDU_opcodes_T_57 ? 6'h1c : _IDU_opcodes_T_221; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_223 = _IDU_opcodes_T_55 ? 6'h1b : _IDU_opcodes_T_222; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_224 = _IDU_opcodes_T_53 ? 6'h1a : _IDU_opcodes_T_223; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_225 = _IDU_opcodes_T_51 ? 6'h19 : _IDU_opcodes_T_224; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_226 = _IDU_opcodes_T_49 ? 6'h18 : _IDU_opcodes_T_225; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_227 = _IDU_opcodes_T_47 ? 6'h17 : _IDU_opcodes_T_226; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_228 = _IDU_opcodes_T_45 ? 6'h16 : _IDU_opcodes_T_227; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_229 = _IDU_opcodes_T_43 ? 6'h15 : _IDU_opcodes_T_228; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_230 = _IDU_opcodes_T_41 ? 6'h14 : _IDU_opcodes_T_229; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_231 = _IDU_opcodes_T_39 ? 6'h13 : _IDU_opcodes_T_230; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_232 = _IDU_opcodes_T_37 ? 6'h12 : _IDU_opcodes_T_231; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_233 = _IDU_opcodes_T_35 ? 6'h11 : _IDU_opcodes_T_232; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_234 = _IDU_opcodes_T_33 ? 6'h10 : _IDU_opcodes_T_233; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_235 = _IDU_opcodes_T_31 ? 6'hf : _IDU_opcodes_T_234; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_236 = _IDU_opcodes_T_29 ? 6'he : _IDU_opcodes_T_235; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_237 = _IDU_opcodes_T_27 ? 6'hd : _IDU_opcodes_T_236; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_238 = _IDU_opcodes_T_25 ? 6'hc : _IDU_opcodes_T_237; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_239 = _IDU_opcodes_T_23 ? 6'hb : _IDU_opcodes_T_238; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_240 = _IDU_opcodes_T_21 ? 6'ha : _IDU_opcodes_T_239; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_241 = _IDU_opcodes_T_19 ? 6'h9 : _IDU_opcodes_T_240; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_242 = _IDU_opcodes_T_17 ? 6'h8 : _IDU_opcodes_T_241; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_243 = _IDU_opcodes_T_15 ? 6'h7 : _IDU_opcodes_T_242; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_244 = _IDU_opcodes_T_13 ? 6'h3f : _IDU_opcodes_T_243; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_245 = _IDU_opcodes_T_11 ? 6'h6 : _IDU_opcodes_T_244; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_246 = _IDU_opcodes_T_9 ? 6'h5 : _IDU_opcodes_T_245; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_247 = _IDU_opcodes_T_7 ? 6'h4 : _IDU_opcodes_T_246; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_248 = _IDU_opcodes_T_5 ? 6'h3 : _IDU_opcodes_T_247; // @[Lookup.scala 34:39]
  wire [5:0] _IDU_opcodes_T_249 = _IDU_opcodes_T_3 ? 6'h2 : _IDU_opcodes_T_248; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_272 = _IDU_opcodes_T_81 ? 4'ha : 4'h0; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_273 = _IDU_opcodes_T_79 ? 4'h6 : _IDU_opcodes_T_272; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_274 = _IDU_opcodes_T_77 ? 4'hb : _IDU_opcodes_T_273; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_275 = _IDU_opcodes_T_75 ? 4'h0 : _IDU_opcodes_T_274; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_276 = _IDU_opcodes_T_73 ? 4'h0 : _IDU_opcodes_T_275; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_277 = _IDU_opcodes_T_71 ? 4'h0 : _IDU_opcodes_T_276; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_278 = _IDU_opcodes_T_69 ? 4'h0 : _IDU_opcodes_T_277; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_279 = _IDU_opcodes_T_67 ? 4'h0 : _IDU_opcodes_T_278; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_280 = _IDU_opcodes_T_65 ? 4'h0 : _IDU_opcodes_T_279; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_281 = _IDU_opcodes_T_63 ? 4'h0 : _IDU_opcodes_T_280; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_282 = _IDU_opcodes_T_61 ? 4'h0 : _IDU_opcodes_T_281; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_283 = _IDU_opcodes_T_59 ? 4'h0 : _IDU_opcodes_T_282; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_284 = _IDU_opcodes_T_57 ? 4'h0 : _IDU_opcodes_T_283; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_285 = _IDU_opcodes_T_55 ? 4'h0 : _IDU_opcodes_T_284; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_286 = _IDU_opcodes_T_53 ? 4'h0 : _IDU_opcodes_T_285; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_287 = _IDU_opcodes_T_51 ? 4'h0 : _IDU_opcodes_T_286; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_288 = _IDU_opcodes_T_49 ? 4'h0 : _IDU_opcodes_T_287; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_289 = _IDU_opcodes_T_47 ? 4'h0 : _IDU_opcodes_T_288; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_290 = _IDU_opcodes_T_45 ? 4'h0 : _IDU_opcodes_T_289; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_291 = _IDU_opcodes_T_43 ? 4'h0 : _IDU_opcodes_T_290; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_292 = _IDU_opcodes_T_41 ? 4'h0 : _IDU_opcodes_T_291; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_293 = _IDU_opcodes_T_39 ? 4'h0 : _IDU_opcodes_T_292; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_294 = _IDU_opcodes_T_37 ? 4'h0 : _IDU_opcodes_T_293; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_295 = _IDU_opcodes_T_35 ? 4'h9 : _IDU_opcodes_T_294; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_296 = _IDU_opcodes_T_33 ? 4'h8 : _IDU_opcodes_T_295; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_297 = _IDU_opcodes_T_31 ? 4'h7 : _IDU_opcodes_T_296; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_298 = _IDU_opcodes_T_29 ? 4'h4 : _IDU_opcodes_T_297; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_299 = _IDU_opcodes_T_27 ? 4'h2 : _IDU_opcodes_T_298; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_300 = _IDU_opcodes_T_25 ? 4'h5 : _IDU_opcodes_T_299; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_301 = _IDU_opcodes_T_23 ? 4'h3 : _IDU_opcodes_T_300; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_302 = _IDU_opcodes_T_21 ? 4'h1 : _IDU_opcodes_T_301; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_303 = _IDU_opcodes_T_19 ? 4'h0 : _IDU_opcodes_T_302; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_304 = _IDU_opcodes_T_17 ? 4'h0 : _IDU_opcodes_T_303; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_305 = _IDU_opcodes_T_15 ? 4'h0 : _IDU_opcodes_T_304; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_306 = _IDU_opcodes_T_13 ? 4'h0 : _IDU_opcodes_T_305; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_307 = _IDU_opcodes_T_11 ? 4'h0 : _IDU_opcodes_T_306; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_308 = _IDU_opcodes_T_9 ? 4'h0 : _IDU_opcodes_T_307; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_309 = _IDU_opcodes_T_7 ? 4'h0 : _IDU_opcodes_T_308; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_310 = _IDU_opcodes_T_5 ? 4'h0 : _IDU_opcodes_T_309; // @[Lookup.scala 34:39]
  wire [3:0] _IDU_opcodes_T_311 = _IDU_opcodes_T_3 ? 4'h0 : _IDU_opcodes_T_310; // @[Lookup.scala 34:39]
  wire [3:0] IDU_opcodes_2 = _IDU_opcodes_T_1 ? 4'h0 : _IDU_opcodes_T_311; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_241 = _IDU_opcodes_T_19 ? 1'h0 : 1'h1; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_242 = _IDU_opcodes_T_17 ? 1'h0 : _IDU_switchs_T_241; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_243 = _IDU_opcodes_T_15 ? 1'h0 : _IDU_switchs_T_242; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_244 = _IDU_opcodes_T_13 ? 1'h0 : _IDU_switchs_T_243; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_245 = _IDU_opcodes_T_11 ? 1'h0 : _IDU_switchs_T_244; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_246 = _IDU_opcodes_T_9 ? 1'h0 : _IDU_switchs_T_245; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_247 = _IDU_opcodes_T_7 ? 1'h0 : _IDU_switchs_T_246; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_248 = _IDU_opcodes_T_5 ? 1'h0 : _IDU_switchs_T_247; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_272 = _IDU_opcodes_T_81 ? 1'h0 : _IDU_opcodes_T_83 | (_IDU_opcodes_T_85 | (_IDU_opcodes_T_87 | (
    _IDU_opcodes_T_89 | (_IDU_opcodes_T_91 | (_IDU_opcodes_T_83 | (_IDU_opcodes_T_95 | (_IDU_opcodes_T_97 | (
    _IDU_opcodes_T_99 | (_IDU_opcodes_T_101 | (_IDU_opcodes_T_103 | (_IDU_opcodes_T_105 | (_IDU_opcodes_T_107 | (
    _IDU_opcodes_T_109 | (_IDU_opcodes_T_111 | (_IDU_opcodes_T_113 | (_IDU_opcodes_T_115 | (_IDU_opcodes_T_117 | (
    _IDU_opcodes_T_119 | (_IDU_opcodes_T_121 | (_IDU_opcodes_T_123 | _IDU_opcodes_T_125)))))))))))))))))))); // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_275 = _IDU_opcodes_T_75 ? 1'h0 : _IDU_opcodes_T_77 | (_IDU_opcodes_T_79 | _IDU_switchs_T_272); // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_295 = _IDU_opcodes_T_35 ? 1'h0 : _IDU_opcodes_T_37 | (_IDU_opcodes_T_39 | (_IDU_opcodes_T_41 | (
    _IDU_opcodes_T_43 | (_IDU_opcodes_T_45 | (_IDU_opcodes_T_47 | (_IDU_opcodes_T_49 | (_IDU_opcodes_T_51 | (
    _IDU_opcodes_T_53 | (_IDU_opcodes_T_55 | (_IDU_opcodes_T_57 | (_IDU_opcodes_T_59 | (_IDU_opcodes_T_61 | (
    _IDU_opcodes_T_63 | (_IDU_opcodes_T_65 | (_IDU_opcodes_T_67 | (_IDU_opcodes_T_69 | (_IDU_opcodes_T_71 | (
    _IDU_opcodes_T_73 | _IDU_switchs_T_275)))))))))))))))))); // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_296 = _IDU_opcodes_T_33 ? 1'h0 : _IDU_switchs_T_295; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_297 = _IDU_opcodes_T_31 ? 1'h0 : _IDU_switchs_T_296; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_303 = _IDU_opcodes_T_19 ? 1'h0 : _IDU_opcodes_T_21 | (_IDU_opcodes_T_23 | (_IDU_opcodes_T_25 | (
    _IDU_opcodes_T_27 | (_IDU_opcodes_T_29 | _IDU_switchs_T_297)))); // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_304 = _IDU_opcodes_T_17 ? 1'h0 : _IDU_switchs_T_303; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_305 = _IDU_opcodes_T_15 ? 1'h0 : _IDU_switchs_T_304; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_306 = _IDU_opcodes_T_13 ? 1'h0 : _IDU_switchs_T_305; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_307 = _IDU_opcodes_T_11 ? 1'h0 : _IDU_switchs_T_306; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_308 = _IDU_opcodes_T_9 ? 1'h0 : _IDU_switchs_T_307; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_312 = _IDU_opcodes_T_125 ? 1'h0 : 1'h1; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_313 = _IDU_opcodes_T_123 ? 1'h0 : _IDU_switchs_T_312; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_314 = _IDU_opcodes_T_121 ? 1'h0 : _IDU_switchs_T_313; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_315 = _IDU_opcodes_T_119 ? 1'h0 : _IDU_switchs_T_314; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_316 = _IDU_opcodes_T_117 ? 1'h0 : _IDU_switchs_T_315; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_317 = _IDU_opcodes_T_115 ? 1'h0 : _IDU_switchs_T_316; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_318 = _IDU_opcodes_T_113 ? 1'h0 : _IDU_switchs_T_317; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_319 = _IDU_opcodes_T_111 ? 1'h0 : _IDU_switchs_T_318; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_320 = _IDU_opcodes_T_109 ? 1'h0 : _IDU_switchs_T_319; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_321 = _IDU_opcodes_T_107 ? 1'h0 : _IDU_switchs_T_320; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_322 = _IDU_opcodes_T_105 ? 1'h0 : _IDU_switchs_T_321; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_323 = _IDU_opcodes_T_103 ? 1'h0 : _IDU_switchs_T_322; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_324 = _IDU_opcodes_T_101 ? 1'h0 : _IDU_switchs_T_323; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_325 = _IDU_opcodes_T_99 ? 1'h0 : _IDU_switchs_T_324; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_326 = _IDU_opcodes_T_97 ? 1'h0 : _IDU_switchs_T_325; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_327 = _IDU_opcodes_T_95 ? 1'h0 : _IDU_switchs_T_326; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_328 = _IDU_opcodes_T_83 ? 1'h0 : _IDU_switchs_T_327; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_329 = _IDU_opcodes_T_91 ? 1'h0 : _IDU_switchs_T_328; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_330 = _IDU_opcodes_T_89 ? 1'h0 : _IDU_switchs_T_329; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_331 = _IDU_opcodes_T_87 ? 1'h0 : _IDU_switchs_T_330; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_332 = _IDU_opcodes_T_85 ? 1'h0 : _IDU_switchs_T_331; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_333 = _IDU_opcodes_T_83 ? 1'h0 : _IDU_switchs_T_332; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_334 = _IDU_opcodes_T_81 ? 1'h0 : _IDU_switchs_T_333; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_335 = _IDU_opcodes_T_79 ? 1'h0 : _IDU_switchs_T_334; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_336 = _IDU_opcodes_T_77 ? 1'h0 : _IDU_switchs_T_335; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_337 = _IDU_opcodes_T_75 ? 1'h0 : _IDU_switchs_T_336; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_338 = _IDU_opcodes_T_73 ? 1'h0 : _IDU_switchs_T_337; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_339 = _IDU_opcodes_T_71 ? 1'h0 : _IDU_switchs_T_338; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_340 = _IDU_opcodes_T_69 ? 1'h0 : _IDU_switchs_T_339; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_341 = _IDU_opcodes_T_67 ? 1'h0 : _IDU_switchs_T_340; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_342 = _IDU_opcodes_T_65 ? 1'h0 : _IDU_switchs_T_341; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_343 = _IDU_opcodes_T_63 ? 1'h0 : _IDU_switchs_T_342; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_344 = _IDU_opcodes_T_61 ? 1'h0 : _IDU_switchs_T_343; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_345 = _IDU_opcodes_T_59 ? 1'h0 : _IDU_switchs_T_344; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_346 = _IDU_opcodes_T_57 ? 1'h0 : _IDU_switchs_T_345; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_347 = _IDU_opcodes_T_55 ? 1'h0 : _IDU_switchs_T_346; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_348 = _IDU_opcodes_T_53 ? 1'h0 : _IDU_switchs_T_347; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_349 = _IDU_opcodes_T_51 ? 1'h0 : _IDU_switchs_T_348; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_350 = _IDU_opcodes_T_49 ? 1'h0 : _IDU_switchs_T_349; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_351 = _IDU_opcodes_T_47 ? 1'h0 : _IDU_switchs_T_350; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_352 = _IDU_opcodes_T_45 ? 1'h0 : _IDU_switchs_T_351; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_353 = _IDU_opcodes_T_43 ? 1'h0 : _IDU_switchs_T_352; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_354 = _IDU_opcodes_T_41 ? 1'h0 : _IDU_switchs_T_353; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_355 = _IDU_opcodes_T_39 ? 1'h0 : _IDU_switchs_T_354; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_356 = _IDU_opcodes_T_37 ? 1'h0 : _IDU_switchs_T_355; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_357 = _IDU_opcodes_T_35 ? 1'h0 : _IDU_switchs_T_356; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_358 = _IDU_opcodes_T_33 ? 1'h0 : _IDU_switchs_T_357; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_359 = _IDU_opcodes_T_31 ? 1'h0 : _IDU_switchs_T_358; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_360 = _IDU_opcodes_T_29 ? 1'h0 : _IDU_switchs_T_359; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_361 = _IDU_opcodes_T_27 ? 1'h0 : _IDU_switchs_T_360; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_362 = _IDU_opcodes_T_25 ? 1'h0 : _IDU_switchs_T_361; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_363 = _IDU_opcodes_T_23 ? 1'h0 : _IDU_switchs_T_362; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_364 = _IDU_opcodes_T_21 ? 1'h0 : _IDU_switchs_T_363; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_365 = _IDU_opcodes_T_19 ? 1'h0 : _IDU_switchs_T_364; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_366 = _IDU_opcodes_T_17 ? 1'h0 : _IDU_switchs_T_365; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_367 = _IDU_opcodes_T_15 ? 1'h0 : _IDU_switchs_T_366; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_368 = _IDU_opcodes_T_13 ? 1'h0 : _IDU_switchs_T_367; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_369 = _IDU_opcodes_T_11 ? 1'h0 : _IDU_switchs_T_368; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_370 = _IDU_opcodes_T_9 ? 1'h0 : _IDU_switchs_T_369; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_371 = _IDU_opcodes_T_7 ? 1'h0 : _IDU_switchs_T_370; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_372 = _IDU_opcodes_T_5 ? 1'h0 : _IDU_switchs_T_371; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_373 = _IDU_opcodes_T_3 ? 1'h0 : _IDU_switchs_T_372; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_400 = _IDU_opcodes_T_73 ? 1'h0 : _IDU_opcodes_T_75 | _IDU_switchs_T_336; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_401 = _IDU_opcodes_T_71 ? 1'h0 : _IDU_switchs_T_400; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_402 = _IDU_opcodes_T_69 ? 1'h0 : _IDU_switchs_T_401; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_403 = _IDU_opcodes_T_67 ? 1'h0 : _IDU_switchs_T_402; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_404 = _IDU_opcodes_T_65 ? 1'h0 : _IDU_switchs_T_403; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_405 = _IDU_opcodes_T_63 ? 1'h0 : _IDU_switchs_T_404; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_406 = _IDU_opcodes_T_61 ? 1'h0 : _IDU_switchs_T_405; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_407 = _IDU_opcodes_T_59 ? 1'h0 : _IDU_switchs_T_406; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_408 = _IDU_opcodes_T_57 ? 1'h0 : _IDU_switchs_T_407; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_409 = _IDU_opcodes_T_55 ? 1'h0 : _IDU_switchs_T_408; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_410 = _IDU_opcodes_T_53 ? 1'h0 : _IDU_switchs_T_409; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_411 = _IDU_opcodes_T_51 ? 1'h0 : _IDU_switchs_T_410; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_412 = _IDU_opcodes_T_49 ? 1'h0 : _IDU_switchs_T_411; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_413 = _IDU_opcodes_T_47 ? 1'h0 : _IDU_switchs_T_412; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_414 = _IDU_opcodes_T_45 ? 1'h0 : _IDU_switchs_T_413; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_415 = _IDU_opcodes_T_43 ? 1'h0 : _IDU_switchs_T_414; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_416 = _IDU_opcodes_T_41 ? 1'h0 : _IDU_switchs_T_415; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_417 = _IDU_opcodes_T_39 ? 1'h0 : _IDU_switchs_T_416; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_418 = _IDU_opcodes_T_37 ? 1'h0 : _IDU_switchs_T_417; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_419 = _IDU_opcodes_T_35 ? 1'h0 : _IDU_switchs_T_418; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_420 = _IDU_opcodes_T_33 ? 1'h0 : _IDU_switchs_T_419; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_421 = _IDU_opcodes_T_31 ? 1'h0 : _IDU_switchs_T_420; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_422 = _IDU_opcodes_T_29 ? 1'h0 : _IDU_switchs_T_421; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_423 = _IDU_opcodes_T_27 ? 1'h0 : _IDU_switchs_T_422; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_424 = _IDU_opcodes_T_25 ? 1'h0 : _IDU_switchs_T_423; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_425 = _IDU_opcodes_T_23 ? 1'h0 : _IDU_switchs_T_424; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_426 = _IDU_opcodes_T_21 ? 1'h0 : _IDU_switchs_T_425; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_427 = _IDU_opcodes_T_19 ? 1'h0 : _IDU_switchs_T_426; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_428 = _IDU_opcodes_T_17 ? 1'h0 : _IDU_switchs_T_427; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_429 = _IDU_opcodes_T_15 ? 1'h0 : _IDU_switchs_T_428; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_430 = _IDU_opcodes_T_13 ? 1'h0 : _IDU_switchs_T_429; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_431 = _IDU_opcodes_T_11 ? 1'h0 : _IDU_switchs_T_430; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_432 = _IDU_opcodes_T_9 ? 1'h0 : _IDU_switchs_T_431; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_433 = _IDU_opcodes_T_7 ? 1'h0 : _IDU_switchs_T_432; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_434 = _IDU_opcodes_T_5 ? 1'h0 : _IDU_switchs_T_433; // @[Lookup.scala 34:39]
  wire  _IDU_switchs_T_435 = _IDU_opcodes_T_3 ? 1'h0 : _IDU_switchs_T_434; // @[Lookup.scala 34:39]
  wire  _io_IDU_O_imm_T = IDU_inst_type == 3'h7; // @[npc.scala 384:24]
  wire  _io_IDU_O_imm_T_1 = IDU_inst_type == 3'h6; // @[npc.scala 385:24]
  wire  _io_IDU_O_imm_T_2 = IDU_inst_type == 3'h0; // @[npc.scala 386:24]
  wire  _io_IDU_O_imm_T_3 = IDU_inst_type == 3'h1; // @[npc.scala 387:24]
  wire  _io_IDU_O_imm_T_4 = IDU_inst_type == 3'h2; // @[npc.scala 388:24]
  wire  _io_IDU_O_imm_T_5 = IDU_inst_type == 3'h3; // @[npc.scala 389:24]
  wire  _io_IDU_O_imm_T_6 = IDU_inst_type == 3'h4; // @[npc.scala 390:24]
  wire  _io_IDU_O_imm_T_7 = IDU_inst_type == 3'h5; // @[npc.scala 391:24]
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
  assign io_IDU_O_EXUopcode = _IDU_opcodes_T_1 ? 6'h1 : _IDU_opcodes_T_249; // @[Lookup.scala 34:39]
  assign io_IDU_O_LSUopcode = {{2'd0}, IDU_opcodes_2}; // @[npc.scala 306:31]
  assign io_IDU_O_snpcISdnpc = _IDU_opcodes_T_1 | (_IDU_opcodes_T_3 | _IDU_switchs_T_248); // @[Lookup.scala 34:39]
  assign io_IDU_O_GPRneedWriteBack = _IDU_opcodes_T_1 | (_IDU_opcodes_T_3 | (_IDU_opcodes_T_5 | (_IDU_opcodes_T_7 |
    _IDU_switchs_T_308))); // @[Lookup.scala 34:39]
  assign io_IDU_O_imm = _io_IDU_O_imm_T_15[63:0]; // @[npc.scala 382:18]
  assign io_IDU_O_halt = _IDU_opcodes_T_1 ? 1'h0 : _IDU_switchs_T_435; // @[Lookup.scala 34:39]
  assign io_IDU_O_error = _IDU_opcodes_T_1 ? 1'h0 : _IDU_switchs_T_373; // @[Lookup.scala 34:39]
endmodule
module EXU(
  input  [63:0] io_EXU_I_src1,
  input  [63:0] io_EXU_I_src2,
  input  [63:0] io_EXU_I_imm,
  input  [5:0]  io_EXU_I_opcode,
  input  [63:0] io_EXU_I_currentPC,
  output [63:0] io_EXU_O_result,
  output        io_EXU_O_snpcNEQdnpc,
  output        io_EXU_O_error
);
  wire [63:0] EXU_src2_signed = io_EXU_I_src2; // @[npc.scala 409:41]
  wire [63:0] EXU_imm_signed = io_EXU_I_imm; // @[npc.scala 411:39]
  wire  _io_EXU_O_result_T = io_EXU_I_opcode == 6'h0; // @[npc.scala 416:26]
  wire  _io_EXU_O_result_T_1 = io_EXU_I_opcode == 6'h1; // @[npc.scala 417:26]
  wire  _io_EXU_O_result_T_2 = io_EXU_I_opcode == 6'h2; // @[npc.scala 418:26]
  wire [63:0] _io_EXU_O_result_T_4 = io_EXU_I_currentPC + io_EXU_I_imm; // @[npc.scala 418:79]
  wire  _io_EXU_O_result_T_5 = io_EXU_I_opcode == 6'h3; // @[npc.scala 419:26]
  wire  _io_EXU_O_result_T_8 = io_EXU_I_opcode == 6'h4; // @[npc.scala 420:26]
  wire [63:0] _io_EXU_O_result_T_10 = io_EXU_I_src1 + io_EXU_I_src2; // @[npc.scala 420:79]
  wire  _io_EXU_O_result_T_13 = io_EXU_I_opcode == 6'h5; // @[npc.scala 421:26]
  wire  _io_EXU_O_result_T_16 = io_EXU_I_opcode == 6'h6; // @[npc.scala 422:26]
  wire  _io_EXU_O_result_T_19 = io_EXU_I_opcode == 6'h3f; // @[npc.scala 423:26]
  wire  _io_EXU_O_result_T_22 = io_EXU_I_opcode == 6'h7; // @[npc.scala 424:26]
  wire  _io_EXU_O_result_T_25 = io_EXU_I_opcode == 6'h8; // @[npc.scala 425:26]
  wire  _io_EXU_O_result_T_28 = io_EXU_I_opcode == 6'h9; // @[npc.scala 426:26]
  wire  _io_EXU_O_result_T_31 = io_EXU_I_opcode == 6'ha; // @[npc.scala 427:26]
  wire [63:0] _io_EXU_O_result_T_33 = io_EXU_I_src1 + io_EXU_I_imm; // @[npc.scala 427:78]
  wire  _io_EXU_O_result_T_34 = io_EXU_I_opcode == 6'hb; // @[npc.scala 428:26]
  wire  _io_EXU_O_result_T_37 = io_EXU_I_opcode == 6'hc; // @[npc.scala 429:26]
  wire  _io_EXU_O_result_T_40 = io_EXU_I_opcode == 6'hd; // @[npc.scala 430:26]
  wire  _io_EXU_O_result_T_43 = io_EXU_I_opcode == 6'he; // @[npc.scala 431:26]
  wire  _io_EXU_O_result_T_46 = io_EXU_I_opcode == 6'hf; // @[npc.scala 432:26]
  wire  _io_EXU_O_result_T_49 = io_EXU_I_opcode == 6'h10; // @[npc.scala 433:26]
  wire  _io_EXU_O_result_T_52 = io_EXU_I_opcode == 6'h11; // @[npc.scala 434:26]
  wire  _io_EXU_O_result_T_55 = io_EXU_I_opcode == 6'h12; // @[npc.scala 435:26]
  wire  _io_EXU_O_result_T_58 = io_EXU_I_opcode == 6'h13; // @[npc.scala 436:26]
  wire [63:0] _io_EXU_O_result_T_60 = $signed(io_EXU_I_src1) < $signed(io_EXU_I_imm) ? 64'h1 : 64'h0; // @[npc.scala 436:63]
  wire  _io_EXU_O_result_T_61 = io_EXU_I_opcode == 6'h14; // @[npc.scala 437:26]
  wire [63:0] _io_EXU_O_result_T_63 = io_EXU_I_src1 < io_EXU_I_imm ? 64'h1 : 64'h0; // @[npc.scala 437:63]
  wire  _io_EXU_O_result_T_64 = io_EXU_I_opcode == 6'h15; // @[npc.scala 438:26]
  wire [63:0] _io_EXU_O_result_T_65 = io_EXU_I_src1 ^ io_EXU_I_imm; // @[npc.scala 438:78]
  wire  _io_EXU_O_result_T_66 = io_EXU_I_opcode == 6'h16; // @[npc.scala 439:26]
  wire [63:0] _io_EXU_O_result_T_67 = io_EXU_I_src1 | io_EXU_I_imm; // @[npc.scala 439:78]
  wire  _io_EXU_O_result_T_68 = io_EXU_I_opcode == 6'h17; // @[npc.scala 440:26]
  wire [63:0] _io_EXU_O_result_T_69 = io_EXU_I_src1 & io_EXU_I_imm; // @[npc.scala 440:78]
  wire  _io_EXU_O_result_T_70 = io_EXU_I_opcode == 6'h18; // @[npc.scala 441:26]
  wire [126:0] _GEN_0 = {{63'd0}, io_EXU_I_src1}; // @[npc.scala 441:78]
  wire [126:0] _io_EXU_O_result_T_72 = _GEN_0 << io_EXU_I_imm[5:0]; // @[npc.scala 441:78]
  wire  _io_EXU_O_result_T_73 = io_EXU_I_opcode == 6'h19; // @[npc.scala 442:26]
  wire [63:0] _io_EXU_O_result_T_75 = io_EXU_I_src1 >> io_EXU_I_imm[5:0]; // @[npc.scala 442:78]
  wire  _io_EXU_O_result_T_76 = io_EXU_I_opcode == 6'h1a; // @[npc.scala 443:26]
  wire  _io_EXU_O_result_T_79 = io_EXU_I_opcode == 6'h1b; // @[npc.scala 444:26]
  wire  _io_EXU_O_result_T_82 = io_EXU_I_opcode == 6'h1c; // @[npc.scala 445:26]
  wire [63:0] _io_EXU_O_result_T_84 = io_EXU_I_src1 - io_EXU_I_src2; // @[npc.scala 445:78]
  wire  _io_EXU_O_result_T_85 = io_EXU_I_opcode == 6'h1d; // @[npc.scala 446:26]
  wire [126:0] _GEN_1 = {{63'd0}, io_EXU_I_src1}; // @[npc.scala 446:78]
  wire [126:0] _io_EXU_O_result_T_87 = _GEN_1 << io_EXU_I_src2[5:0]; // @[npc.scala 446:78]
  wire  _io_EXU_O_result_T_88 = io_EXU_I_opcode == 6'h1e; // @[npc.scala 447:26]
  wire  _io_EXU_O_result_T_89 = $signed(io_EXU_I_src1) < $signed(io_EXU_I_src2); // @[npc.scala 447:80]
  wire [63:0] _io_EXU_O_result_T_90 = $signed(io_EXU_I_src1) < $signed(io_EXU_I_src2) ? 64'h1 : 64'h0; // @[npc.scala 447:63]
  wire  _io_EXU_O_result_T_91 = io_EXU_I_opcode == 6'h1f; // @[npc.scala 448:26]
  wire  _io_EXU_O_result_T_92 = io_EXU_I_src1 < io_EXU_I_src2; // @[npc.scala 448:82]
  wire [63:0] _io_EXU_O_result_T_93 = io_EXU_I_src1 < io_EXU_I_src2 ? 64'h1 : 64'h0; // @[npc.scala 448:63]
  wire  _io_EXU_O_result_T_94 = io_EXU_I_opcode == 6'h20; // @[npc.scala 449:26]
  wire [63:0] _io_EXU_O_result_T_95 = io_EXU_I_src1 ^ io_EXU_I_src2; // @[npc.scala 449:78]
  wire  _io_EXU_O_result_T_96 = io_EXU_I_opcode == 6'h21; // @[npc.scala 450:26]
  wire [63:0] _io_EXU_O_result_T_98 = io_EXU_I_src1 >> io_EXU_I_src2[5:0]; // @[npc.scala 450:78]
  wire  _io_EXU_O_result_T_99 = io_EXU_I_opcode == 6'h22; // @[npc.scala 451:26]
  wire [63:0] _io_EXU_O_result_T_102 = $signed(io_EXU_I_src1) >>> EXU_src2_signed[5:0]; // @[npc.scala 451:102]
  wire  _io_EXU_O_result_T_103 = io_EXU_I_opcode == 6'h23; // @[npc.scala 452:26]
  wire [63:0] _io_EXU_O_result_T_104 = io_EXU_I_src1 | io_EXU_I_src2; // @[npc.scala 452:78]
  wire  _io_EXU_O_result_T_105 = io_EXU_I_opcode == 6'h24; // @[npc.scala 453:26]
  wire [63:0] _io_EXU_O_result_T_106 = io_EXU_I_src1 & io_EXU_I_src2; // @[npc.scala 453:78]
  wire  _io_EXU_O_result_T_107 = io_EXU_I_opcode == 6'h25; // @[npc.scala 454:26]
  wire  _io_EXU_O_result_T_110 = io_EXU_I_opcode == 6'h26; // @[npc.scala 455:26]
  wire  _io_EXU_O_result_T_113 = io_EXU_I_opcode == 6'h27; // @[npc.scala 456:26]
  wire  _io_EXU_O_result_T_116 = io_EXU_I_opcode == 6'h28; // @[npc.scala 457:26]
  wire [31:0] _io_EXU_O_result_T_121 = _io_EXU_O_result_T_33[31] ? 32'hffffffff : 32'h0; // @[Bitwise.scala 77:12]
  wire [63:0] _io_EXU_O_result_T_125 = {_io_EXU_O_result_T_121,_io_EXU_O_result_T_33[31:0]}; // @[Cat.scala 33:92]
  wire  _io_EXU_O_result_T_126 = io_EXU_I_opcode == 6'h29; // @[npc.scala 458:26]
  wire [94:0] _GEN_2 = {{31'd0}, io_EXU_I_src1}; // @[npc.scala 458:92]
  wire [94:0] _io_EXU_O_result_T_128 = _GEN_2 << io_EXU_I_imm[4:0]; // @[npc.scala 458:92]
  wire [31:0] _io_EXU_O_result_T_131 = _io_EXU_O_result_T_128[31] ? 32'hffffffff : 32'h0; // @[Bitwise.scala 77:12]
  wire [63:0] _io_EXU_O_result_T_135 = {_io_EXU_O_result_T_131,_io_EXU_O_result_T_128[31:0]}; // @[Cat.scala 33:92]
  wire  _io_EXU_O_result_T_136 = io_EXU_I_opcode == 6'h2a; // @[npc.scala 459:26]
  wire [63:0] _io_EXU_O_result_T_138 = io_EXU_I_src1 >> io_EXU_I_imm[4:0]; // @[npc.scala 459:92]
  wire [31:0] _io_EXU_O_result_T_141 = _io_EXU_O_result_T_138[31] ? 32'hffffffff : 32'h0; // @[Bitwise.scala 77:12]
  wire [63:0] _io_EXU_O_result_T_145 = {_io_EXU_O_result_T_141,_io_EXU_O_result_T_138[31:0]}; // @[Cat.scala 33:92]
  wire  _io_EXU_O_result_T_146 = io_EXU_I_opcode == 6'h2b; // @[npc.scala 460:26]
  wire [63:0] _io_EXU_O_result_T_148 = $signed(io_EXU_I_src1) >>> io_EXU_I_imm[4:0]; // @[npc.scala 460:90]
  wire [31:0] _io_EXU_O_result_T_151 = _io_EXU_O_result_T_148[31] ? 32'hffffffff : 32'h0; // @[Bitwise.scala 77:12]
  wire [63:0] _io_EXU_O_result_T_153 = $signed(io_EXU_I_src1) >>> EXU_imm_signed[4:0]; // @[npc.scala 460:140]
  wire [63:0] _io_EXU_O_result_T_155 = {_io_EXU_O_result_T_151,_io_EXU_O_result_T_153[31:0]}; // @[Cat.scala 33:92]
  wire  _io_EXU_O_result_T_156 = io_EXU_I_opcode == 6'h2c; // @[npc.scala 461:26]
  wire [31:0] _io_EXU_O_result_T_161 = _io_EXU_O_result_T_10[31] ? 32'hffffffff : 32'h0; // @[Bitwise.scala 77:12]
  wire [63:0] _io_EXU_O_result_T_165 = {_io_EXU_O_result_T_161,_io_EXU_O_result_T_10[31:0]}; // @[Cat.scala 33:92]
  wire  _io_EXU_O_result_T_166 = io_EXU_I_opcode == 6'h2d; // @[npc.scala 462:26]
  wire [31:0] _io_EXU_O_result_T_171 = _io_EXU_O_result_T_84[31] ? 32'hffffffff : 32'h0; // @[Bitwise.scala 77:12]
  wire [63:0] _io_EXU_O_result_T_175 = {_io_EXU_O_result_T_171,_io_EXU_O_result_T_84[31:0]}; // @[Cat.scala 33:92]
  wire  _io_EXU_O_result_T_176 = io_EXU_I_opcode == 6'h2e; // @[npc.scala 463:26]
  wire [94:0] _GEN_3 = {{31'd0}, io_EXU_I_src1}; // @[npc.scala 463:92]
  wire [94:0] _io_EXU_O_result_T_178 = _GEN_3 << io_EXU_I_src2[4:0]; // @[npc.scala 463:92]
  wire [31:0] _io_EXU_O_result_T_181 = _io_EXU_O_result_T_178[31] ? 32'hffffffff : 32'h0; // @[Bitwise.scala 77:12]
  wire [63:0] _io_EXU_O_result_T_185 = {_io_EXU_O_result_T_181,_io_EXU_O_result_T_128[31:0]}; // @[Cat.scala 33:92]
  wire  _io_EXU_O_result_T_186 = io_EXU_I_opcode == 6'h2f; // @[npc.scala 464:26]
  wire [63:0] _io_EXU_O_result_T_188 = io_EXU_I_src1 >> io_EXU_I_src2[4:0]; // @[npc.scala 464:92]
  wire [31:0] _io_EXU_O_result_T_191 = _io_EXU_O_result_T_188[31] ? 32'hffffffff : 32'h0; // @[Bitwise.scala 77:12]
  wire [63:0] _io_EXU_O_result_T_195 = {_io_EXU_O_result_T_191,_io_EXU_O_result_T_138[31:0]}; // @[Cat.scala 33:92]
  wire  _io_EXU_O_result_T_196 = io_EXU_I_opcode == 6'h30; // @[npc.scala 465:26]
  wire [63:0] _io_EXU_O_result_T_198 = $signed(io_EXU_I_src1) >>> io_EXU_I_src2[4:0]; // @[npc.scala 465:90]
  wire [31:0] _io_EXU_O_result_T_201 = _io_EXU_O_result_T_198[31] ? 32'hffffffff : 32'h0; // @[Bitwise.scala 77:12]
  wire [63:0] _io_EXU_O_result_T_205 = {_io_EXU_O_result_T_201,_io_EXU_O_result_T_153[31:0]}; // @[Cat.scala 33:92]
  wire  _io_EXU_O_result_T_206 = io_EXU_I_opcode == 6'h31; // @[npc.scala 466:26]
  wire [127:0] _io_EXU_O_result_T_207 = io_EXU_I_src1 * io_EXU_I_src2; // @[npc.scala 466:79]
  wire  _io_EXU_O_result_T_209 = io_EXU_I_opcode == 6'h32; // @[npc.scala 467:26]
  wire [127:0] _io_EXU_O_result_T_210 = $signed(io_EXU_I_src1) * $signed(io_EXU_I_src2); // @[npc.scala 467:77]
  wire  _io_EXU_O_result_T_212 = io_EXU_I_opcode == 6'h33; // @[npc.scala 468:26]
  wire [64:0] _io_EXU_O_result_T_213 = {1'b0,$signed(io_EXU_I_src2)}; // @[npc.scala 468:77]
  wire [128:0] _io_EXU_O_result_T_214 = $signed(io_EXU_I_src1) * $signed(_io_EXU_O_result_T_213); // @[npc.scala 468:77]
  wire [127:0] _io_EXU_O_result_T_216 = _io_EXU_O_result_T_214[127:0]; // @[npc.scala 468:77]
  wire  _io_EXU_O_result_T_218 = io_EXU_I_opcode == 6'h34; // @[npc.scala 469:26]
  wire  _io_EXU_O_result_T_221 = io_EXU_I_opcode == 6'h35; // @[npc.scala 470:26]
  wire [64:0] _io_EXU_O_result_T_222 = $signed(io_EXU_I_src1) / $signed(io_EXU_I_src2); // @[npc.scala 470:77]
  wire [64:0] _io_EXU_O_result_T_223 = $signed(io_EXU_I_src1) / $signed(io_EXU_I_src2); // @[npc.scala 470:97]
  wire  _io_EXU_O_result_T_224 = io_EXU_I_opcode == 6'h36; // @[npc.scala 471:26]
  wire [63:0] _io_EXU_O_result_T_225 = io_EXU_I_src1 / io_EXU_I_src2; // @[npc.scala 471:79]
  wire  _io_EXU_O_result_T_226 = io_EXU_I_opcode == 6'h37; // @[npc.scala 472:26]
  wire [63:0] _io_EXU_O_result_T_227 = $signed(io_EXU_I_src1) % $signed(io_EXU_I_src2); // @[npc.scala 472:77]
  wire [63:0] _io_EXU_O_result_T_228 = $signed(io_EXU_I_src1) % $signed(io_EXU_I_src2); // @[npc.scala 472:97]
  wire  _io_EXU_O_result_T_229 = io_EXU_I_opcode == 6'h38; // @[npc.scala 473:26]
  wire [63:0] _io_EXU_O_result_T_230 = io_EXU_I_src1 % io_EXU_I_src2; // @[npc.scala 473:79]
  wire  _io_EXU_O_result_T_231 = io_EXU_I_opcode == 6'h39; // @[npc.scala 474:26]
  wire [31:0] _io_EXU_O_result_T_235 = _io_EXU_O_result_T_207[31] ? 32'hffffffff : 32'h0; // @[Bitwise.scala 77:12]
  wire [63:0] _io_EXU_O_result_T_238 = {_io_EXU_O_result_T_235,_io_EXU_O_result_T_207[31:0]}; // @[Cat.scala 33:92]
  wire  _io_EXU_O_result_T_239 = io_EXU_I_opcode == 6'h3a; // @[npc.scala 475:26]
  wire [31:0] _io_EXU_O_result_T_243 = _io_EXU_O_result_T_222[31] ? 32'hffffffff : 32'h0; // @[Bitwise.scala 77:12]
  wire [63:0] _io_EXU_O_result_T_246 = {_io_EXU_O_result_T_243,_io_EXU_O_result_T_222[31:0]}; // @[Cat.scala 33:92]
  wire  _io_EXU_O_result_T_247 = io_EXU_I_opcode == 6'h3b; // @[npc.scala 476:26]
  wire [31:0] _io_EXU_O_result_T_251 = _io_EXU_O_result_T_225[31] ? 32'hffffffff : 32'h0; // @[Bitwise.scala 77:12]
  wire [63:0] _io_EXU_O_result_T_254 = {_io_EXU_O_result_T_251,_io_EXU_O_result_T_225[31:0]}; // @[Cat.scala 33:92]
  wire  _io_EXU_O_result_T_255 = io_EXU_I_opcode == 6'h3c; // @[npc.scala 477:26]
  wire [31:0] _io_EXU_O_result_T_259 = _io_EXU_O_result_T_227[31] ? 32'hffffffff : 32'h0; // @[Bitwise.scala 77:12]
  wire [63:0] _io_EXU_O_result_T_262 = {_io_EXU_O_result_T_259,_io_EXU_O_result_T_227[31:0]}; // @[Cat.scala 33:92]
  wire  _io_EXU_O_result_T_263 = io_EXU_I_opcode == 6'h3d; // @[npc.scala 478:26]
  wire [31:0] _io_EXU_O_result_T_267 = _io_EXU_O_result_T_230[31] ? 32'hffffffff : 32'h0; // @[Bitwise.scala 77:12]
  wire [63:0] _io_EXU_O_result_T_270 = {_io_EXU_O_result_T_267,_io_EXU_O_result_T_230[31:0]}; // @[Cat.scala 33:92]
  wire [63:0] _io_EXU_O_result_T_271 = _io_EXU_O_result_T_263 ? _io_EXU_O_result_T_270 : 64'h0; // @[Mux.scala 101:16]
  wire [63:0] _io_EXU_O_result_T_272 = _io_EXU_O_result_T_255 ? _io_EXU_O_result_T_262 : _io_EXU_O_result_T_271; // @[Mux.scala 101:16]
  wire [63:0] _io_EXU_O_result_T_273 = _io_EXU_O_result_T_247 ? _io_EXU_O_result_T_254 : _io_EXU_O_result_T_272; // @[Mux.scala 101:16]
  wire [63:0] _io_EXU_O_result_T_274 = _io_EXU_O_result_T_239 ? _io_EXU_O_result_T_246 : _io_EXU_O_result_T_273; // @[Mux.scala 101:16]
  wire [63:0] _io_EXU_O_result_T_275 = _io_EXU_O_result_T_231 ? _io_EXU_O_result_T_238 : _io_EXU_O_result_T_274; // @[Mux.scala 101:16]
  wire [63:0] _io_EXU_O_result_T_276 = _io_EXU_O_result_T_229 ? _io_EXU_O_result_T_230 : _io_EXU_O_result_T_275; // @[Mux.scala 101:16]
  wire [63:0] _io_EXU_O_result_T_277 = _io_EXU_O_result_T_226 ? _io_EXU_O_result_T_228 : _io_EXU_O_result_T_276; // @[Mux.scala 101:16]
  wire [63:0] _io_EXU_O_result_T_278 = _io_EXU_O_result_T_224 ? _io_EXU_O_result_T_225 : _io_EXU_O_result_T_277; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_279 = _io_EXU_O_result_T_221 ? _io_EXU_O_result_T_223 : {{1'd0}, _io_EXU_O_result_T_278
    }; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_280 = _io_EXU_O_result_T_218 ? {{1'd0}, _io_EXU_O_result_T_207[127:64]} :
    _io_EXU_O_result_T_279; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_281 = _io_EXU_O_result_T_212 ? {{1'd0}, _io_EXU_O_result_T_216[127:64]} :
    _io_EXU_O_result_T_280; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_282 = _io_EXU_O_result_T_209 ? {{1'd0}, _io_EXU_O_result_T_210[127:64]} :
    _io_EXU_O_result_T_281; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_283 = _io_EXU_O_result_T_206 ? {{1'd0}, _io_EXU_O_result_T_207[63:0]} :
    _io_EXU_O_result_T_282; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_284 = _io_EXU_O_result_T_196 ? {{1'd0}, _io_EXU_O_result_T_205} :
    _io_EXU_O_result_T_283; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_285 = _io_EXU_O_result_T_186 ? {{1'd0}, _io_EXU_O_result_T_195} :
    _io_EXU_O_result_T_284; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_286 = _io_EXU_O_result_T_176 ? {{1'd0}, _io_EXU_O_result_T_185} :
    _io_EXU_O_result_T_285; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_287 = _io_EXU_O_result_T_166 ? {{1'd0}, _io_EXU_O_result_T_175} :
    _io_EXU_O_result_T_286; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_288 = _io_EXU_O_result_T_156 ? {{1'd0}, _io_EXU_O_result_T_165} :
    _io_EXU_O_result_T_287; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_289 = _io_EXU_O_result_T_146 ? {{1'd0}, _io_EXU_O_result_T_155} :
    _io_EXU_O_result_T_288; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_290 = _io_EXU_O_result_T_136 ? {{1'd0}, _io_EXU_O_result_T_145} :
    _io_EXU_O_result_T_289; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_291 = _io_EXU_O_result_T_126 ? {{1'd0}, _io_EXU_O_result_T_135} :
    _io_EXU_O_result_T_290; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_292 = _io_EXU_O_result_T_116 ? {{1'd0}, _io_EXU_O_result_T_125} :
    _io_EXU_O_result_T_291; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_293 = _io_EXU_O_result_T_113 ? {{1'd0}, _io_EXU_O_result_T_33} : _io_EXU_O_result_T_292
    ; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_294 = _io_EXU_O_result_T_110 ? {{1'd0}, _io_EXU_O_result_T_33} : _io_EXU_O_result_T_293
    ; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_295 = _io_EXU_O_result_T_107 ? {{1'd0}, _io_EXU_O_result_T_33} : _io_EXU_O_result_T_294
    ; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_296 = _io_EXU_O_result_T_105 ? {{1'd0}, _io_EXU_O_result_T_106} :
    _io_EXU_O_result_T_295; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_297 = _io_EXU_O_result_T_103 ? {{1'd0}, _io_EXU_O_result_T_104} :
    _io_EXU_O_result_T_296; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_298 = _io_EXU_O_result_T_99 ? {{1'd0}, _io_EXU_O_result_T_102} : _io_EXU_O_result_T_297
    ; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_299 = _io_EXU_O_result_T_96 ? {{1'd0}, _io_EXU_O_result_T_98} : _io_EXU_O_result_T_298; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_300 = _io_EXU_O_result_T_94 ? {{1'd0}, _io_EXU_O_result_T_95} : _io_EXU_O_result_T_299; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_301 = _io_EXU_O_result_T_91 ? {{1'd0}, _io_EXU_O_result_T_93} : _io_EXU_O_result_T_300; // @[Mux.scala 101:16]
  wire [64:0] _io_EXU_O_result_T_302 = _io_EXU_O_result_T_88 ? {{1'd0}, _io_EXU_O_result_T_90} : _io_EXU_O_result_T_301; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_303 = _io_EXU_O_result_T_85 ? _io_EXU_O_result_T_87 : {{62'd0}, _io_EXU_O_result_T_302
    }; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_304 = _io_EXU_O_result_T_82 ? {{63'd0}, _io_EXU_O_result_T_84} :
    _io_EXU_O_result_T_303; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_305 = _io_EXU_O_result_T_79 ? {{63'd0}, _io_EXU_O_result_T_10} :
    _io_EXU_O_result_T_304; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_306 = _io_EXU_O_result_T_76 ? {{63'd0}, _io_EXU_O_result_T_75} :
    _io_EXU_O_result_T_305; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_307 = _io_EXU_O_result_T_73 ? {{63'd0}, _io_EXU_O_result_T_75} :
    _io_EXU_O_result_T_306; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_308 = _io_EXU_O_result_T_70 ? _io_EXU_O_result_T_72 : _io_EXU_O_result_T_307; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_309 = _io_EXU_O_result_T_68 ? {{63'd0}, _io_EXU_O_result_T_69} :
    _io_EXU_O_result_T_308; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_310 = _io_EXU_O_result_T_66 ? {{63'd0}, _io_EXU_O_result_T_67} :
    _io_EXU_O_result_T_309; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_311 = _io_EXU_O_result_T_64 ? {{63'd0}, _io_EXU_O_result_T_65} :
    _io_EXU_O_result_T_310; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_312 = _io_EXU_O_result_T_61 ? {{63'd0}, _io_EXU_O_result_T_63} :
    _io_EXU_O_result_T_311; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_313 = _io_EXU_O_result_T_58 ? {{63'd0}, _io_EXU_O_result_T_60} :
    _io_EXU_O_result_T_312; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_314 = _io_EXU_O_result_T_55 ? {{63'd0}, _io_EXU_O_result_T_33} :
    _io_EXU_O_result_T_313; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_315 = _io_EXU_O_result_T_52 ? {{63'd0}, _io_EXU_O_result_T_33} :
    _io_EXU_O_result_T_314; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_316 = _io_EXU_O_result_T_49 ? {{63'd0}, _io_EXU_O_result_T_33} :
    _io_EXU_O_result_T_315; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_317 = _io_EXU_O_result_T_46 ? {{63'd0}, _io_EXU_O_result_T_33} :
    _io_EXU_O_result_T_316; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_318 = _io_EXU_O_result_T_43 ? {{63'd0}, _io_EXU_O_result_T_33} :
    _io_EXU_O_result_T_317; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_319 = _io_EXU_O_result_T_40 ? {{63'd0}, _io_EXU_O_result_T_33} :
    _io_EXU_O_result_T_318; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_320 = _io_EXU_O_result_T_37 ? {{63'd0}, _io_EXU_O_result_T_33} :
    _io_EXU_O_result_T_319; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_321 = _io_EXU_O_result_T_34 ? {{63'd0}, _io_EXU_O_result_T_33} :
    _io_EXU_O_result_T_320; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_322 = _io_EXU_O_result_T_31 ? {{63'd0}, _io_EXU_O_result_T_33} :
    _io_EXU_O_result_T_321; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_323 = _io_EXU_O_result_T_28 ? {{63'd0}, _io_EXU_O_result_T_4} : _io_EXU_O_result_T_322
    ; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_324 = _io_EXU_O_result_T_25 ? {{63'd0}, _io_EXU_O_result_T_4} : _io_EXU_O_result_T_323
    ; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_325 = _io_EXU_O_result_T_22 ? {{63'd0}, _io_EXU_O_result_T_4} : _io_EXU_O_result_T_324
    ; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_326 = _io_EXU_O_result_T_19 ? {{63'd0}, _io_EXU_O_result_T_4} : _io_EXU_O_result_T_325
    ; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_327 = _io_EXU_O_result_T_16 ? {{63'd0}, _io_EXU_O_result_T_4} : _io_EXU_O_result_T_326
    ; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_328 = _io_EXU_O_result_T_13 ? {{63'd0}, _io_EXU_O_result_T_4} : _io_EXU_O_result_T_327
    ; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_329 = _io_EXU_O_result_T_8 ? 127'h0 : _io_EXU_O_result_T_328; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_330 = _io_EXU_O_result_T_5 ? {{63'd0}, _io_EXU_O_result_T_4} : _io_EXU_O_result_T_329; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_331 = _io_EXU_O_result_T_2 ? {{63'd0}, _io_EXU_O_result_T_4} : _io_EXU_O_result_T_330; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_332 = _io_EXU_O_result_T_1 ? {{63'd0}, io_EXU_I_imm} : _io_EXU_O_result_T_331; // @[Mux.scala 101:16]
  wire [126:0] _io_EXU_O_result_T_333 = _io_EXU_O_result_T ? 127'h0 : _io_EXU_O_result_T_332; // @[Mux.scala 101:16]
  wire  _io_EXU_O_snpcNEQdnpc_T_4 = ~(io_EXU_I_src1 == io_EXU_I_src2); // @[npc.scala 485:60]
  wire  _io_EXU_O_snpcNEQdnpc_T_7 = ~(io_EXU_I_src1 != io_EXU_I_src2); // @[npc.scala 486:60]
  wire  _io_EXU_O_snpcNEQdnpc_T_10 = ~_io_EXU_O_result_T_89; // @[npc.scala 487:60]
  wire  _io_EXU_O_snpcNEQdnpc_T_13 = ~($signed(io_EXU_I_src1) >= $signed(io_EXU_I_src2)); // @[npc.scala 488:60]
  wire  _io_EXU_O_snpcNEQdnpc_T_16 = ~_io_EXU_O_result_T_92; // @[npc.scala 489:60]
  wire  _io_EXU_O_snpcNEQdnpc_T_19 = ~(io_EXU_I_src1 >= io_EXU_I_src2); // @[npc.scala 490:60]
  wire  _io_EXU_O_snpcNEQdnpc_T_21 = _io_EXU_O_result_T_25 ? _io_EXU_O_snpcNEQdnpc_T_16 : _io_EXU_O_result_T_28 &
    _io_EXU_O_snpcNEQdnpc_T_19; // @[Mux.scala 101:16]
  wire  _io_EXU_O_snpcNEQdnpc_T_22 = _io_EXU_O_result_T_22 ? _io_EXU_O_snpcNEQdnpc_T_13 : _io_EXU_O_snpcNEQdnpc_T_21; // @[Mux.scala 101:16]
  wire  _io_EXU_O_snpcNEQdnpc_T_23 = _io_EXU_O_result_T_19 ? _io_EXU_O_snpcNEQdnpc_T_10 : _io_EXU_O_snpcNEQdnpc_T_22; // @[Mux.scala 101:16]
  wire  _io_EXU_O_snpcNEQdnpc_T_24 = _io_EXU_O_result_T_16 ? _io_EXU_O_snpcNEQdnpc_T_7 : _io_EXU_O_snpcNEQdnpc_T_23; // @[Mux.scala 101:16]
  wire  _io_EXU_O_snpcNEQdnpc_T_25 = _io_EXU_O_result_T_13 ? _io_EXU_O_snpcNEQdnpc_T_4 : _io_EXU_O_snpcNEQdnpc_T_24; // @[Mux.scala 101:16]
  assign io_EXU_O_result = _io_EXU_O_result_T_333[63:0]; // @[npc.scala 414:21]
  assign io_EXU_O_snpcNEQdnpc = _io_EXU_O_result_T_5 | (_io_EXU_O_result_T_8 | _io_EXU_O_snpcNEQdnpc_T_25); // @[Mux.scala 101:16]
  assign io_EXU_O_error = io_EXU_I_opcode == 6'h0; // @[npc.scala 495:26]
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
  output        io_LSU_O_error,
  output [1:0]  io_LSU_O_len
);
  wire  _io_LSU_O_result_T = io_LSU_I_opcode == 6'h0; // @[npc.scala 533:26]
  wire  _io_LSU_O_result_T_1 = io_LSU_I_opcode == 6'h1; // @[npc.scala 534:26]
  wire [55:0] _io_LSU_O_result_T_4 = io_LSU_I_memR[7] ? 56'hffffffffffffff : 56'h0; // @[Bitwise.scala 77:12]
  wire [119:0] _io_LSU_O_result_T_5 = {_io_LSU_O_result_T_4,io_LSU_I_memR}; // @[Cat.scala 33:92]
  wire  _io_LSU_O_result_T_6 = io_LSU_I_opcode == 6'h2; // @[npc.scala 535:26]
  wire [119:0] _io_LSU_O_result_T_8 = {56'h0,io_LSU_I_memR}; // @[Cat.scala 33:92]
  wire  _io_LSU_O_result_T_9 = io_LSU_I_opcode == 6'h3; // @[npc.scala 536:26]
  wire [47:0] _io_LSU_O_result_T_12 = io_LSU_I_memR[15] ? 48'hffffffffffff : 48'h0; // @[Bitwise.scala 77:12]
  wire [111:0] _io_LSU_O_result_T_13 = {_io_LSU_O_result_T_12,io_LSU_I_memR}; // @[Cat.scala 33:92]
  wire  _io_LSU_O_result_T_14 = io_LSU_I_opcode == 6'h4; // @[npc.scala 537:26]
  wire [111:0] _io_LSU_O_result_T_16 = {48'h0,io_LSU_I_memR}; // @[Cat.scala 33:92]
  wire  _io_LSU_O_result_T_17 = io_LSU_I_opcode == 6'h5; // @[npc.scala 538:26]
  wire [31:0] _io_LSU_O_result_T_20 = io_LSU_I_memR[31] ? 32'hffffffff : 32'h0; // @[Bitwise.scala 77:12]
  wire [95:0] _io_LSU_O_result_T_21 = {_io_LSU_O_result_T_20,io_LSU_I_memR}; // @[Cat.scala 33:92]
  wire  _io_LSU_O_result_T_22 = io_LSU_I_opcode == 6'hb; // @[npc.scala 539:26]
  wire [95:0] _io_LSU_O_result_T_24 = {32'h0,io_LSU_I_memR}; // @[Cat.scala 33:92]
  wire  _io_LSU_O_result_T_25 = io_LSU_I_opcode == 6'h6; // @[npc.scala 540:26]
  wire  _io_LSU_O_result_T_26 = io_LSU_I_opcode == 6'h7; // @[npc.scala 541:26]
  wire  _io_LSU_O_result_T_27 = io_LSU_I_opcode == 6'h8; // @[npc.scala 542:26]
  wire  _io_LSU_O_result_T_28 = io_LSU_I_opcode == 6'h9; // @[npc.scala 543:26]
  wire  _io_LSU_O_result_T_29 = io_LSU_I_opcode == 6'ha; // @[npc.scala 544:26]
  wire [63:0] _io_LSU_O_result_T_34 = _io_LSU_O_result_T_25 ? io_LSU_I_memR : 64'h0; // @[Mux.scala 101:16]
  wire [95:0] _io_LSU_O_result_T_35 = _io_LSU_O_result_T_22 ? _io_LSU_O_result_T_24 : {{32'd0}, _io_LSU_O_result_T_34}; // @[Mux.scala 101:16]
  wire [95:0] _io_LSU_O_result_T_36 = _io_LSU_O_result_T_17 ? _io_LSU_O_result_T_21 : _io_LSU_O_result_T_35; // @[Mux.scala 101:16]
  wire [111:0] _io_LSU_O_result_T_37 = _io_LSU_O_result_T_14 ? _io_LSU_O_result_T_16 : {{16'd0}, _io_LSU_O_result_T_36}; // @[Mux.scala 101:16]
  wire [111:0] _io_LSU_O_result_T_38 = _io_LSU_O_result_T_9 ? _io_LSU_O_result_T_13 : _io_LSU_O_result_T_37; // @[Mux.scala 101:16]
  wire [119:0] _io_LSU_O_result_T_39 = _io_LSU_O_result_T_6 ? _io_LSU_O_result_T_8 : {{8'd0}, _io_LSU_O_result_T_38}; // @[Mux.scala 101:16]
  wire [119:0] _io_LSU_O_result_T_40 = _io_LSU_O_result_T_1 ? _io_LSU_O_result_T_5 : _io_LSU_O_result_T_39; // @[Mux.scala 101:16]
  wire [119:0] _io_LSU_O_result_T_41 = _io_LSU_O_result_T ? 120'h0 : _io_LSU_O_result_T_40; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memAddr_T_12 = _io_LSU_O_result_T_29 ? io_LSU_I_src1 : 64'h0; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memAddr_T_13 = _io_LSU_O_result_T_28 ? io_LSU_I_src1 : _io_LSU_O_memAddr_T_12; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memAddr_T_14 = _io_LSU_O_result_T_27 ? io_LSU_I_src1 : _io_LSU_O_memAddr_T_13; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memAddr_T_15 = _io_LSU_O_result_T_26 ? io_LSU_I_src1 : _io_LSU_O_memAddr_T_14; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memAddr_T_16 = _io_LSU_O_result_T_25 ? io_LSU_I_src1 : _io_LSU_O_memAddr_T_15; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memAddr_T_17 = _io_LSU_O_result_T_22 ? io_LSU_I_src1 : _io_LSU_O_memAddr_T_16; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memAddr_T_18 = _io_LSU_O_result_T_17 ? io_LSU_I_src1 : _io_LSU_O_memAddr_T_17; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memAddr_T_19 = _io_LSU_O_result_T_14 ? io_LSU_I_src1 : _io_LSU_O_memAddr_T_18; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memAddr_T_20 = _io_LSU_O_result_T_9 ? io_LSU_I_src1 : _io_LSU_O_memAddr_T_19; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memAddr_T_21 = _io_LSU_O_result_T_6 ? io_LSU_I_src1 : _io_LSU_O_memAddr_T_20; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memAddr_T_22 = _io_LSU_O_result_T_1 ? io_LSU_I_src1 : _io_LSU_O_memAddr_T_21; // @[Mux.scala 101:16]
  wire  _io_LSU_O_memRW_T_16 = _io_LSU_O_result_T_25 ? 1'h0 : _io_LSU_O_result_T_26 | (_io_LSU_O_result_T_27 | (
    _io_LSU_O_result_T_28 | _io_LSU_O_result_T_29)); // @[Mux.scala 101:16]
  wire  _io_LSU_O_memRW_T_17 = _io_LSU_O_result_T_22 ? 1'h0 : _io_LSU_O_memRW_T_16; // @[Mux.scala 101:16]
  wire  _io_LSU_O_memRW_T_18 = _io_LSU_O_result_T_17 ? 1'h0 : _io_LSU_O_memRW_T_17; // @[Mux.scala 101:16]
  wire  _io_LSU_O_memRW_T_19 = _io_LSU_O_result_T_14 ? 1'h0 : _io_LSU_O_memRW_T_18; // @[Mux.scala 101:16]
  wire  _io_LSU_O_memRW_T_20 = _io_LSU_O_result_T_9 ? 1'h0 : _io_LSU_O_memRW_T_19; // @[Mux.scala 101:16]
  wire  _io_LSU_O_memRW_T_21 = _io_LSU_O_result_T_6 ? 1'h0 : _io_LSU_O_memRW_T_20; // @[Mux.scala 101:16]
  wire  _io_LSU_O_memRW_T_22 = _io_LSU_O_result_T_1 ? 1'h0 : _io_LSU_O_memRW_T_21; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memW_T_12 = _io_LSU_O_result_T_29 ? io_LSU_I_src2 : 64'h0; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memW_T_13 = _io_LSU_O_result_T_28 ? io_LSU_I_src2 : _io_LSU_O_memW_T_12; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memW_T_14 = _io_LSU_O_result_T_27 ? io_LSU_I_src2 : _io_LSU_O_memW_T_13; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memW_T_15 = _io_LSU_O_result_T_26 ? io_LSU_I_src2 : _io_LSU_O_memW_T_14; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memW_T_16 = _io_LSU_O_result_T_25 ? 64'h0 : _io_LSU_O_memW_T_15; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memW_T_17 = _io_LSU_O_result_T_22 ? 64'h0 : _io_LSU_O_memW_T_16; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memW_T_18 = _io_LSU_O_result_T_17 ? 64'h0 : _io_LSU_O_memW_T_17; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memW_T_19 = _io_LSU_O_result_T_14 ? 64'h0 : _io_LSU_O_memW_T_18; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memW_T_20 = _io_LSU_O_result_T_9 ? 64'h0 : _io_LSU_O_memW_T_19; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memW_T_21 = _io_LSU_O_result_T_6 ? 64'h0 : _io_LSU_O_memW_T_20; // @[Mux.scala 101:16]
  wire [63:0] _io_LSU_O_memW_T_22 = _io_LSU_O_result_T_1 ? 64'h0 : _io_LSU_O_memW_T_21; // @[Mux.scala 101:16]
  wire  _io_LSU_O_error_T_12 = _io_LSU_O_result_T_29 ? 1'h0 : 1'h1; // @[Mux.scala 101:16]
  wire  _io_LSU_O_error_T_13 = _io_LSU_O_result_T_28 ? 1'h0 : _io_LSU_O_error_T_12; // @[Mux.scala 101:16]
  wire  _io_LSU_O_error_T_14 = _io_LSU_O_result_T_27 ? 1'h0 : _io_LSU_O_error_T_13; // @[Mux.scala 101:16]
  wire  _io_LSU_O_error_T_15 = _io_LSU_O_result_T_26 ? 1'h0 : _io_LSU_O_error_T_14; // @[Mux.scala 101:16]
  wire  _io_LSU_O_error_T_16 = _io_LSU_O_result_T_25 ? 1'h0 : _io_LSU_O_error_T_15; // @[Mux.scala 101:16]
  wire  _io_LSU_O_error_T_17 = _io_LSU_O_result_T_22 ? 1'h0 : _io_LSU_O_error_T_16; // @[Mux.scala 101:16]
  wire  _io_LSU_O_error_T_18 = _io_LSU_O_result_T_17 ? 1'h0 : _io_LSU_O_error_T_17; // @[Mux.scala 101:16]
  wire  _io_LSU_O_error_T_19 = _io_LSU_O_result_T_14 ? 1'h0 : _io_LSU_O_error_T_18; // @[Mux.scala 101:16]
  wire  _io_LSU_O_error_T_20 = _io_LSU_O_result_T_9 ? 1'h0 : _io_LSU_O_error_T_19; // @[Mux.scala 101:16]
  wire  _io_LSU_O_error_T_21 = _io_LSU_O_result_T_6 ? 1'h0 : _io_LSU_O_error_T_20; // @[Mux.scala 101:16]
  wire  _io_LSU_O_error_T_22 = _io_LSU_O_result_T_1 ? 1'h0 : _io_LSU_O_error_T_21; // @[Mux.scala 101:16]
  wire [1:0] _io_LSU_O_len_T_12 = _io_LSU_O_result_T_29 ? 2'h3 : 2'h0; // @[Mux.scala 101:16]
  wire [1:0] _io_LSU_O_len_T_13 = _io_LSU_O_result_T_28 ? 2'h2 : _io_LSU_O_len_T_12; // @[Mux.scala 101:16]
  wire [1:0] _io_LSU_O_len_T_14 = _io_LSU_O_result_T_27 ? 2'h1 : _io_LSU_O_len_T_13; // @[Mux.scala 101:16]
  wire [1:0] _io_LSU_O_len_T_15 = _io_LSU_O_result_T_26 ? 2'h0 : _io_LSU_O_len_T_14; // @[Mux.scala 101:16]
  wire [1:0] _io_LSU_O_len_T_16 = _io_LSU_O_result_T_25 ? 2'h3 : _io_LSU_O_len_T_15; // @[Mux.scala 101:16]
  wire [1:0] _io_LSU_O_len_T_17 = _io_LSU_O_result_T_22 ? 2'h2 : _io_LSU_O_len_T_16; // @[Mux.scala 101:16]
  wire [1:0] _io_LSU_O_len_T_18 = _io_LSU_O_result_T_17 ? 2'h2 : _io_LSU_O_len_T_17; // @[Mux.scala 101:16]
  wire [1:0] _io_LSU_O_len_T_19 = _io_LSU_O_result_T_14 ? 2'h1 : _io_LSU_O_len_T_18; // @[Mux.scala 101:16]
  wire [1:0] _io_LSU_O_len_T_20 = _io_LSU_O_result_T_9 ? 2'h1 : _io_LSU_O_len_T_19; // @[Mux.scala 101:16]
  wire [1:0] _io_LSU_O_len_T_21 = _io_LSU_O_result_T_6 ? 2'h0 : _io_LSU_O_len_T_20; // @[Mux.scala 101:16]
  wire [1:0] _io_LSU_O_len_T_22 = _io_LSU_O_result_T_1 ? 2'h0 : _io_LSU_O_len_T_21; // @[Mux.scala 101:16]
  assign io_LSU_O_result = _io_LSU_O_result_T_41[63:0]; // @[npc.scala 531:21]
  assign io_LSU_O_memAddr = _io_LSU_O_result_T ? 64'h0 : _io_LSU_O_memAddr_T_22; // @[Mux.scala 101:16]
  assign io_LSU_O_accessMem = _io_LSU_O_result_T ? 1'h0 : _io_LSU_O_result_T_1 | (_io_LSU_O_result_T_6 | (
    _io_LSU_O_result_T_9 | (_io_LSU_O_result_T_14 | (_io_LSU_O_result_T_17 | (_io_LSU_O_result_T_22 | (
    _io_LSU_O_result_T_25 | (_io_LSU_O_result_T_26 | (_io_LSU_O_result_T_27 | (_io_LSU_O_result_T_28 |
    _io_LSU_O_result_T_29))))))))); // @[Mux.scala 101:16]
  assign io_LSU_O_memRW = _io_LSU_O_result_T ? 1'h0 : _io_LSU_O_memRW_T_22; // @[Mux.scala 101:16]
  assign io_LSU_O_memW = _io_LSU_O_result_T ? 64'h0 : _io_LSU_O_memW_T_22; // @[Mux.scala 101:16]
  assign io_LSU_O_error = _io_LSU_O_result_T ? 1'h0 : _io_LSU_O_error_T_22; // @[Mux.scala 101:16]
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
  assign io_WBU_O_GPRidx = io_WBU_I_rd; // @[npc.scala 654:21]
  assign io_WBU_O_GPRWriteBack = io_WBU_I_LSUenable ? io_WBU_I_LSUresult : io_WBU_I_EXUresult; // @[npc.scala 655:33]
endmodule
module PCU(
  input  [63:0] io_PCU_I_CurrentPC,
  input  [63:0] io_PCU_I_imm,
  input         io_PCU_I_willJump,
  output [63:0] io_PCU_O_DynamicNextPC
);
  wire [63:0] _io_PCU_O_StaticNextPC_T_1 = io_PCU_I_CurrentPC + 64'h4; // @[npc.scala 508:49]
  wire [63:0] _io_PCU_O_DynamicNextPC_T_1 = io_PCU_I_CurrentPC + io_PCU_I_imm; // @[npc.scala 509:73]
  assign io_PCU_O_DynamicNextPC = io_PCU_I_willJump ? _io_PCU_O_DynamicNextPC_T_1 : _io_PCU_O_StaticNextPC_T_1; // @[npc.scala 509:34]
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
  output        io_NPC_error,
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
  reg [63:0] GPR [0:31]; // @[npc.scala 713:18]
  wire  GPR_npcb_src1_MPORT_en; // @[npc.scala 713:18]
  wire [4:0] GPR_npcb_src1_MPORT_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_npcb_src1_MPORT_data; // @[npc.scala 713:18]
  wire  GPR_npcb_src2_MPORT_en; // @[npc.scala 713:18]
  wire [4:0] GPR_npcb_src2_MPORT_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_npcb_src2_MPORT_data; // @[npc.scala 713:18]
  wire  GPR_MPORT_1_en; // @[npc.scala 713:18]
  wire [4:0] GPR_MPORT_1_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_MPORT_1_data; // @[npc.scala 713:18]
  wire  GPR_io_NPC_GPR00_MPORT_en; // @[npc.scala 713:18]
  wire [4:0] GPR_io_NPC_GPR00_MPORT_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_io_NPC_GPR00_MPORT_data; // @[npc.scala 713:18]
  wire  GPR_io_NPC_GPR01_MPORT_en; // @[npc.scala 713:18]
  wire [4:0] GPR_io_NPC_GPR01_MPORT_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_io_NPC_GPR01_MPORT_data; // @[npc.scala 713:18]
  wire  GPR_io_NPC_GPR02_MPORT_en; // @[npc.scala 713:18]
  wire [4:0] GPR_io_NPC_GPR02_MPORT_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_io_NPC_GPR02_MPORT_data; // @[npc.scala 713:18]
  wire  GPR_io_NPC_GPR03_MPORT_en; // @[npc.scala 713:18]
  wire [4:0] GPR_io_NPC_GPR03_MPORT_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_io_NPC_GPR03_MPORT_data; // @[npc.scala 713:18]
  wire  GPR_io_NPC_GPR04_MPORT_en; // @[npc.scala 713:18]
  wire [4:0] GPR_io_NPC_GPR04_MPORT_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_io_NPC_GPR04_MPORT_data; // @[npc.scala 713:18]
  wire  GPR_io_NPC_GPR05_MPORT_en; // @[npc.scala 713:18]
  wire [4:0] GPR_io_NPC_GPR05_MPORT_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_io_NPC_GPR05_MPORT_data; // @[npc.scala 713:18]
  wire  GPR_io_NPC_GPR06_MPORT_en; // @[npc.scala 713:18]
  wire [4:0] GPR_io_NPC_GPR06_MPORT_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_io_NPC_GPR06_MPORT_data; // @[npc.scala 713:18]
  wire  GPR_io_NPC_GPR07_MPORT_en; // @[npc.scala 713:18]
  wire [4:0] GPR_io_NPC_GPR07_MPORT_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_io_NPC_GPR07_MPORT_data; // @[npc.scala 713:18]
  wire  GPR_io_NPC_GPR08_MPORT_en; // @[npc.scala 713:18]
  wire [4:0] GPR_io_NPC_GPR08_MPORT_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_io_NPC_GPR08_MPORT_data; // @[npc.scala 713:18]
  wire  GPR_io_NPC_GPR09_MPORT_en; // @[npc.scala 713:18]
  wire [4:0] GPR_io_NPC_GPR09_MPORT_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_io_NPC_GPR09_MPORT_data; // @[npc.scala 713:18]
  wire  GPR_io_NPC_GPR10_MPORT_en; // @[npc.scala 713:18]
  wire [4:0] GPR_io_NPC_GPR10_MPORT_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_io_NPC_GPR10_MPORT_data; // @[npc.scala 713:18]
  wire  GPR_io_NPC_GPR11_MPORT_en; // @[npc.scala 713:18]
  wire [4:0] GPR_io_NPC_GPR11_MPORT_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_io_NPC_GPR11_MPORT_data; // @[npc.scala 713:18]
  wire  GPR_io_NPC_GPR12_MPORT_en; // @[npc.scala 713:18]
  wire [4:0] GPR_io_NPC_GPR12_MPORT_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_io_NPC_GPR12_MPORT_data; // @[npc.scala 713:18]
  wire  GPR_io_NPC_GPR13_MPORT_en; // @[npc.scala 713:18]
  wire [4:0] GPR_io_NPC_GPR13_MPORT_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_io_NPC_GPR13_MPORT_data; // @[npc.scala 713:18]
  wire  GPR_io_NPC_GPR14_MPORT_en; // @[npc.scala 713:18]
  wire [4:0] GPR_io_NPC_GPR14_MPORT_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_io_NPC_GPR14_MPORT_data; // @[npc.scala 713:18]
  wire  GPR_io_NPC_GPR15_MPORT_en; // @[npc.scala 713:18]
  wire [4:0] GPR_io_NPC_GPR15_MPORT_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_io_NPC_GPR15_MPORT_data; // @[npc.scala 713:18]
  wire  GPR_io_NPC_GPR16_MPORT_en; // @[npc.scala 713:18]
  wire [4:0] GPR_io_NPC_GPR16_MPORT_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_io_NPC_GPR16_MPORT_data; // @[npc.scala 713:18]
  wire  GPR_io_NPC_GPR17_MPORT_en; // @[npc.scala 713:18]
  wire [4:0] GPR_io_NPC_GPR17_MPORT_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_io_NPC_GPR17_MPORT_data; // @[npc.scala 713:18]
  wire  GPR_io_NPC_GPR18_MPORT_en; // @[npc.scala 713:18]
  wire [4:0] GPR_io_NPC_GPR18_MPORT_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_io_NPC_GPR18_MPORT_data; // @[npc.scala 713:18]
  wire  GPR_io_NPC_GPR19_MPORT_en; // @[npc.scala 713:18]
  wire [4:0] GPR_io_NPC_GPR19_MPORT_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_io_NPC_GPR19_MPORT_data; // @[npc.scala 713:18]
  wire  GPR_io_NPC_GPR20_MPORT_en; // @[npc.scala 713:18]
  wire [4:0] GPR_io_NPC_GPR20_MPORT_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_io_NPC_GPR20_MPORT_data; // @[npc.scala 713:18]
  wire  GPR_io_NPC_GPR21_MPORT_en; // @[npc.scala 713:18]
  wire [4:0] GPR_io_NPC_GPR21_MPORT_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_io_NPC_GPR21_MPORT_data; // @[npc.scala 713:18]
  wire  GPR_io_NPC_GPR22_MPORT_en; // @[npc.scala 713:18]
  wire [4:0] GPR_io_NPC_GPR22_MPORT_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_io_NPC_GPR22_MPORT_data; // @[npc.scala 713:18]
  wire  GPR_io_NPC_GPR23_MPORT_en; // @[npc.scala 713:18]
  wire [4:0] GPR_io_NPC_GPR23_MPORT_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_io_NPC_GPR23_MPORT_data; // @[npc.scala 713:18]
  wire  GPR_io_NPC_GPR24_MPORT_en; // @[npc.scala 713:18]
  wire [4:0] GPR_io_NPC_GPR24_MPORT_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_io_NPC_GPR24_MPORT_data; // @[npc.scala 713:18]
  wire  GPR_io_NPC_GPR25_MPORT_en; // @[npc.scala 713:18]
  wire [4:0] GPR_io_NPC_GPR25_MPORT_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_io_NPC_GPR25_MPORT_data; // @[npc.scala 713:18]
  wire  GPR_io_NPC_GPR26_MPORT_en; // @[npc.scala 713:18]
  wire [4:0] GPR_io_NPC_GPR26_MPORT_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_io_NPC_GPR26_MPORT_data; // @[npc.scala 713:18]
  wire  GPR_io_NPC_GPR27_MPORT_en; // @[npc.scala 713:18]
  wire [4:0] GPR_io_NPC_GPR27_MPORT_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_io_NPC_GPR27_MPORT_data; // @[npc.scala 713:18]
  wire  GPR_io_NPC_GPR28_MPORT_en; // @[npc.scala 713:18]
  wire [4:0] GPR_io_NPC_GPR28_MPORT_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_io_NPC_GPR28_MPORT_data; // @[npc.scala 713:18]
  wire  GPR_io_NPC_GPR29_MPORT_en; // @[npc.scala 713:18]
  wire [4:0] GPR_io_NPC_GPR29_MPORT_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_io_NPC_GPR29_MPORT_data; // @[npc.scala 713:18]
  wire  GPR_io_NPC_GPR30_MPORT_en; // @[npc.scala 713:18]
  wire [4:0] GPR_io_NPC_GPR30_MPORT_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_io_NPC_GPR30_MPORT_data; // @[npc.scala 713:18]
  wire  GPR_io_NPC_GPR31_MPORT_en; // @[npc.scala 713:18]
  wire [4:0] GPR_io_NPC_GPR31_MPORT_addr; // @[npc.scala 713:18]
  wire [63:0] GPR_io_NPC_GPR31_MPORT_data; // @[npc.scala 713:18]
  wire [63:0] GPR_MPORT_data; // @[npc.scala 713:18]
  wire [4:0] GPR_MPORT_addr; // @[npc.scala 713:18]
  wire  GPR_MPORT_mask; // @[npc.scala 713:18]
  wire  GPR_MPORT_en; // @[npc.scala 713:18]
  wire [31:0] npcb_IFU_io_IFU_I_inst; // @[npc.scala 717:26]
  wire [31:0] npcb_IDU_io_IDU_I_inst; // @[npc.scala 723:26]
  wire [4:0] npcb_IDU_io_IDU_O_rs1; // @[npc.scala 723:26]
  wire [4:0] npcb_IDU_io_IDU_O_rs2; // @[npc.scala 723:26]
  wire [4:0] npcb_IDU_io_IDU_O_rd; // @[npc.scala 723:26]
  wire [5:0] npcb_IDU_io_IDU_O_EXUopcode; // @[npc.scala 723:26]
  wire [5:0] npcb_IDU_io_IDU_O_LSUopcode; // @[npc.scala 723:26]
  wire  npcb_IDU_io_IDU_O_snpcISdnpc; // @[npc.scala 723:26]
  wire  npcb_IDU_io_IDU_O_GPRneedWriteBack; // @[npc.scala 723:26]
  wire [63:0] npcb_IDU_io_IDU_O_imm; // @[npc.scala 723:26]
  wire  npcb_IDU_io_IDU_O_halt; // @[npc.scala 723:26]
  wire  npcb_IDU_io_IDU_O_error; // @[npc.scala 723:26]
  wire [63:0] npcb_EXU_io_EXU_I_src1; // @[npc.scala 732:26]
  wire [63:0] npcb_EXU_io_EXU_I_src2; // @[npc.scala 732:26]
  wire [63:0] npcb_EXU_io_EXU_I_imm; // @[npc.scala 732:26]
  wire [5:0] npcb_EXU_io_EXU_I_opcode; // @[npc.scala 732:26]
  wire [63:0] npcb_EXU_io_EXU_I_currentPC; // @[npc.scala 732:26]
  wire [63:0] npcb_EXU_io_EXU_O_result; // @[npc.scala 732:26]
  wire  npcb_EXU_io_EXU_O_snpcNEQdnpc; // @[npc.scala 732:26]
  wire  npcb_EXU_io_EXU_O_error; // @[npc.scala 732:26]
  wire [63:0] npcb_LSU_io_LSU_I_src1; // @[npc.scala 740:26]
  wire [63:0] npcb_LSU_io_LSU_I_src2; // @[npc.scala 740:26]
  wire [5:0] npcb_LSU_io_LSU_I_opcode; // @[npc.scala 740:26]
  wire [63:0] npcb_LSU_io_LSU_O_result; // @[npc.scala 740:26]
  wire [63:0] npcb_LSU_io_LSU_O_memAddr; // @[npc.scala 740:26]
  wire  npcb_LSU_io_LSU_O_accessMem; // @[npc.scala 740:26]
  wire  npcb_LSU_io_LSU_O_memRW; // @[npc.scala 740:26]
  wire [63:0] npcb_LSU_io_LSU_I_memR; // @[npc.scala 740:26]
  wire [63:0] npcb_LSU_io_LSU_O_memW; // @[npc.scala 740:26]
  wire  npcb_LSU_io_LSU_O_error; // @[npc.scala 740:26]
  wire [1:0] npcb_LSU_io_LSU_O_len; // @[npc.scala 740:26]
  wire [63:0] npcb_WBU_io_WBU_I_EXUresult; // @[npc.scala 753:26]
  wire [63:0] npcb_WBU_io_WBU_I_LSUresult; // @[npc.scala 753:26]
  wire  npcb_WBU_io_WBU_I_LSUenable; // @[npc.scala 753:26]
  wire [4:0] npcb_WBU_io_WBU_I_rd; // @[npc.scala 753:26]
  wire [4:0] npcb_WBU_io_WBU_O_GPRidx; // @[npc.scala 753:26]
  wire [63:0] npcb_WBU_io_WBU_O_GPRWriteBack; // @[npc.scala 753:26]
  wire [63:0] npcb_PCU_io_PCU_I_CurrentPC; // @[npc.scala 754:26]
  wire [63:0] npcb_PCU_io_PCU_I_imm; // @[npc.scala 754:26]
  wire  npcb_PCU_io_PCU_I_willJump; // @[npc.scala 754:26]
  wire [63:0] npcb_PCU_io_PCU_O_DynamicNextPC; // @[npc.scala 754:26]
  reg [63:0] PC; // @[npc.scala 711:21]
  wire [63:0] _T_1 = npcb_WBU_io_WBU_O_GPRidx == 5'h0 ? 64'h0 : GPR_MPORT_1_data; // @[npc.scala 714:37]
  IFU npcb_IFU ( // @[npc.scala 717:26]
    .io_IFU_I_inst(npcb_IFU_io_IFU_I_inst)
  );
  IDU npcb_IDU ( // @[npc.scala 723:26]
    .io_IDU_I_inst(npcb_IDU_io_IDU_I_inst),
    .io_IDU_O_rs1(npcb_IDU_io_IDU_O_rs1),
    .io_IDU_O_rs2(npcb_IDU_io_IDU_O_rs2),
    .io_IDU_O_rd(npcb_IDU_io_IDU_O_rd),
    .io_IDU_O_EXUopcode(npcb_IDU_io_IDU_O_EXUopcode),
    .io_IDU_O_LSUopcode(npcb_IDU_io_IDU_O_LSUopcode),
    .io_IDU_O_snpcISdnpc(npcb_IDU_io_IDU_O_snpcISdnpc),
    .io_IDU_O_GPRneedWriteBack(npcb_IDU_io_IDU_O_GPRneedWriteBack),
    .io_IDU_O_imm(npcb_IDU_io_IDU_O_imm),
    .io_IDU_O_halt(npcb_IDU_io_IDU_O_halt),
    .io_IDU_O_error(npcb_IDU_io_IDU_O_error)
  );
  EXU npcb_EXU ( // @[npc.scala 732:26]
    .io_EXU_I_src1(npcb_EXU_io_EXU_I_src1),
    .io_EXU_I_src2(npcb_EXU_io_EXU_I_src2),
    .io_EXU_I_imm(npcb_EXU_io_EXU_I_imm),
    .io_EXU_I_opcode(npcb_EXU_io_EXU_I_opcode),
    .io_EXU_I_currentPC(npcb_EXU_io_EXU_I_currentPC),
    .io_EXU_O_result(npcb_EXU_io_EXU_O_result),
    .io_EXU_O_snpcNEQdnpc(npcb_EXU_io_EXU_O_snpcNEQdnpc),
    .io_EXU_O_error(npcb_EXU_io_EXU_O_error)
  );
  LSU npcb_LSU ( // @[npc.scala 740:26]
    .io_LSU_I_src1(npcb_LSU_io_LSU_I_src1),
    .io_LSU_I_src2(npcb_LSU_io_LSU_I_src2),
    .io_LSU_I_opcode(npcb_LSU_io_LSU_I_opcode),
    .io_LSU_O_result(npcb_LSU_io_LSU_O_result),
    .io_LSU_O_memAddr(npcb_LSU_io_LSU_O_memAddr),
    .io_LSU_O_accessMem(npcb_LSU_io_LSU_O_accessMem),
    .io_LSU_O_memRW(npcb_LSU_io_LSU_O_memRW),
    .io_LSU_I_memR(npcb_LSU_io_LSU_I_memR),
    .io_LSU_O_memW(npcb_LSU_io_LSU_O_memW),
    .io_LSU_O_error(npcb_LSU_io_LSU_O_error),
    .io_LSU_O_len(npcb_LSU_io_LSU_O_len)
  );
  WBU npcb_WBU ( // @[npc.scala 753:26]
    .io_WBU_I_EXUresult(npcb_WBU_io_WBU_I_EXUresult),
    .io_WBU_I_LSUresult(npcb_WBU_io_WBU_I_LSUresult),
    .io_WBU_I_LSUenable(npcb_WBU_io_WBU_I_LSUenable),
    .io_WBU_I_rd(npcb_WBU_io_WBU_I_rd),
    .io_WBU_O_GPRidx(npcb_WBU_io_WBU_O_GPRidx),
    .io_WBU_O_GPRWriteBack(npcb_WBU_io_WBU_O_GPRWriteBack)
  );
  PCU npcb_PCU ( // @[npc.scala 754:26]
    .io_PCU_I_CurrentPC(npcb_PCU_io_PCU_I_CurrentPC),
    .io_PCU_I_imm(npcb_PCU_io_PCU_I_imm),
    .io_PCU_I_willJump(npcb_PCU_io_PCU_I_willJump),
    .io_PCU_O_DynamicNextPC(npcb_PCU_io_PCU_O_DynamicNextPC)
  );
  assign GPR_npcb_src1_MPORT_en = 1'h1;
  assign GPR_npcb_src1_MPORT_addr = npcb_IDU_io_IDU_O_rs1;
  assign GPR_npcb_src1_MPORT_data = GPR[GPR_npcb_src1_MPORT_addr]; // @[npc.scala 713:18]
  assign GPR_npcb_src2_MPORT_en = 1'h1;
  assign GPR_npcb_src2_MPORT_addr = npcb_IDU_io_IDU_O_rs2;
  assign GPR_npcb_src2_MPORT_data = GPR[GPR_npcb_src2_MPORT_addr]; // @[npc.scala 713:18]
  assign GPR_MPORT_1_en = 1'h1;
  assign GPR_MPORT_1_addr = npcb_WBU_io_WBU_O_GPRidx;
  assign GPR_MPORT_1_data = GPR[GPR_MPORT_1_addr]; // @[npc.scala 713:18]
  assign GPR_io_NPC_GPR00_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR00_MPORT_addr = 5'h0;
  assign GPR_io_NPC_GPR00_MPORT_data = GPR[GPR_io_NPC_GPR00_MPORT_addr]; // @[npc.scala 713:18]
  assign GPR_io_NPC_GPR01_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR01_MPORT_addr = 5'h1;
  assign GPR_io_NPC_GPR01_MPORT_data = GPR[GPR_io_NPC_GPR01_MPORT_addr]; // @[npc.scala 713:18]
  assign GPR_io_NPC_GPR02_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR02_MPORT_addr = 5'h2;
  assign GPR_io_NPC_GPR02_MPORT_data = GPR[GPR_io_NPC_GPR02_MPORT_addr]; // @[npc.scala 713:18]
  assign GPR_io_NPC_GPR03_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR03_MPORT_addr = 5'h3;
  assign GPR_io_NPC_GPR03_MPORT_data = GPR[GPR_io_NPC_GPR03_MPORT_addr]; // @[npc.scala 713:18]
  assign GPR_io_NPC_GPR04_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR04_MPORT_addr = 5'h4;
  assign GPR_io_NPC_GPR04_MPORT_data = GPR[GPR_io_NPC_GPR04_MPORT_addr]; // @[npc.scala 713:18]
  assign GPR_io_NPC_GPR05_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR05_MPORT_addr = 5'h5;
  assign GPR_io_NPC_GPR05_MPORT_data = GPR[GPR_io_NPC_GPR05_MPORT_addr]; // @[npc.scala 713:18]
  assign GPR_io_NPC_GPR06_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR06_MPORT_addr = 5'h6;
  assign GPR_io_NPC_GPR06_MPORT_data = GPR[GPR_io_NPC_GPR06_MPORT_addr]; // @[npc.scala 713:18]
  assign GPR_io_NPC_GPR07_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR07_MPORT_addr = 5'h7;
  assign GPR_io_NPC_GPR07_MPORT_data = GPR[GPR_io_NPC_GPR07_MPORT_addr]; // @[npc.scala 713:18]
  assign GPR_io_NPC_GPR08_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR08_MPORT_addr = 5'h8;
  assign GPR_io_NPC_GPR08_MPORT_data = GPR[GPR_io_NPC_GPR08_MPORT_addr]; // @[npc.scala 713:18]
  assign GPR_io_NPC_GPR09_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR09_MPORT_addr = 5'h9;
  assign GPR_io_NPC_GPR09_MPORT_data = GPR[GPR_io_NPC_GPR09_MPORT_addr]; // @[npc.scala 713:18]
  assign GPR_io_NPC_GPR10_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR10_MPORT_addr = 5'ha;
  assign GPR_io_NPC_GPR10_MPORT_data = GPR[GPR_io_NPC_GPR10_MPORT_addr]; // @[npc.scala 713:18]
  assign GPR_io_NPC_GPR11_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR11_MPORT_addr = 5'hb;
  assign GPR_io_NPC_GPR11_MPORT_data = GPR[GPR_io_NPC_GPR11_MPORT_addr]; // @[npc.scala 713:18]
  assign GPR_io_NPC_GPR12_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR12_MPORT_addr = 5'hc;
  assign GPR_io_NPC_GPR12_MPORT_data = GPR[GPR_io_NPC_GPR12_MPORT_addr]; // @[npc.scala 713:18]
  assign GPR_io_NPC_GPR13_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR13_MPORT_addr = 5'hd;
  assign GPR_io_NPC_GPR13_MPORT_data = GPR[GPR_io_NPC_GPR13_MPORT_addr]; // @[npc.scala 713:18]
  assign GPR_io_NPC_GPR14_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR14_MPORT_addr = 5'he;
  assign GPR_io_NPC_GPR14_MPORT_data = GPR[GPR_io_NPC_GPR14_MPORT_addr]; // @[npc.scala 713:18]
  assign GPR_io_NPC_GPR15_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR15_MPORT_addr = 5'hf;
  assign GPR_io_NPC_GPR15_MPORT_data = GPR[GPR_io_NPC_GPR15_MPORT_addr]; // @[npc.scala 713:18]
  assign GPR_io_NPC_GPR16_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR16_MPORT_addr = 5'h10;
  assign GPR_io_NPC_GPR16_MPORT_data = GPR[GPR_io_NPC_GPR16_MPORT_addr]; // @[npc.scala 713:18]
  assign GPR_io_NPC_GPR17_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR17_MPORT_addr = 5'h11;
  assign GPR_io_NPC_GPR17_MPORT_data = GPR[GPR_io_NPC_GPR17_MPORT_addr]; // @[npc.scala 713:18]
  assign GPR_io_NPC_GPR18_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR18_MPORT_addr = 5'h12;
  assign GPR_io_NPC_GPR18_MPORT_data = GPR[GPR_io_NPC_GPR18_MPORT_addr]; // @[npc.scala 713:18]
  assign GPR_io_NPC_GPR19_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR19_MPORT_addr = 5'h13;
  assign GPR_io_NPC_GPR19_MPORT_data = GPR[GPR_io_NPC_GPR19_MPORT_addr]; // @[npc.scala 713:18]
  assign GPR_io_NPC_GPR20_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR20_MPORT_addr = 5'h14;
  assign GPR_io_NPC_GPR20_MPORT_data = GPR[GPR_io_NPC_GPR20_MPORT_addr]; // @[npc.scala 713:18]
  assign GPR_io_NPC_GPR21_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR21_MPORT_addr = 5'h15;
  assign GPR_io_NPC_GPR21_MPORT_data = GPR[GPR_io_NPC_GPR21_MPORT_addr]; // @[npc.scala 713:18]
  assign GPR_io_NPC_GPR22_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR22_MPORT_addr = 5'h16;
  assign GPR_io_NPC_GPR22_MPORT_data = GPR[GPR_io_NPC_GPR22_MPORT_addr]; // @[npc.scala 713:18]
  assign GPR_io_NPC_GPR23_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR23_MPORT_addr = 5'h17;
  assign GPR_io_NPC_GPR23_MPORT_data = GPR[GPR_io_NPC_GPR23_MPORT_addr]; // @[npc.scala 713:18]
  assign GPR_io_NPC_GPR24_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR24_MPORT_addr = 5'h18;
  assign GPR_io_NPC_GPR24_MPORT_data = GPR[GPR_io_NPC_GPR24_MPORT_addr]; // @[npc.scala 713:18]
  assign GPR_io_NPC_GPR25_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR25_MPORT_addr = 5'h19;
  assign GPR_io_NPC_GPR25_MPORT_data = GPR[GPR_io_NPC_GPR25_MPORT_addr]; // @[npc.scala 713:18]
  assign GPR_io_NPC_GPR26_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR26_MPORT_addr = 5'h1a;
  assign GPR_io_NPC_GPR26_MPORT_data = GPR[GPR_io_NPC_GPR26_MPORT_addr]; // @[npc.scala 713:18]
  assign GPR_io_NPC_GPR27_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR27_MPORT_addr = 5'h1b;
  assign GPR_io_NPC_GPR27_MPORT_data = GPR[GPR_io_NPC_GPR27_MPORT_addr]; // @[npc.scala 713:18]
  assign GPR_io_NPC_GPR28_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR28_MPORT_addr = 5'h1c;
  assign GPR_io_NPC_GPR28_MPORT_data = GPR[GPR_io_NPC_GPR28_MPORT_addr]; // @[npc.scala 713:18]
  assign GPR_io_NPC_GPR29_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR29_MPORT_addr = 5'h1d;
  assign GPR_io_NPC_GPR29_MPORT_data = GPR[GPR_io_NPC_GPR29_MPORT_addr]; // @[npc.scala 713:18]
  assign GPR_io_NPC_GPR30_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR30_MPORT_addr = 5'h1e;
  assign GPR_io_NPC_GPR30_MPORT_data = GPR[GPR_io_NPC_GPR30_MPORT_addr]; // @[npc.scala 713:18]
  assign GPR_io_NPC_GPR31_MPORT_en = 1'h1;
  assign GPR_io_NPC_GPR31_MPORT_addr = 5'h1f;
  assign GPR_io_NPC_GPR31_MPORT_data = GPR[GPR_io_NPC_GPR31_MPORT_addr]; // @[npc.scala 713:18]
  assign GPR_MPORT_data = npcb_IDU_io_IDU_O_GPRneedWriteBack ? npcb_WBU_io_WBU_O_GPRWriteBack : _T_1;
  assign GPR_MPORT_addr = npcb_WBU_io_WBU_O_GPRidx;
  assign GPR_MPORT_mask = 1'h1;
  assign GPR_MPORT_en = 1'h1;
  assign io_NPC_sendCurrentPC = PC; // @[npc.scala 720:26]
  assign io_NPC_sendNextPC = PC; // @[npc.scala 765:23]
  assign io_NPC_LSU_O_memAddr = npcb_LSU_io_LSU_O_memAddr; // @[npc.scala 748:26]
  assign io_NPC_LSU_O_accessMem = npcb_LSU_io_LSU_O_accessMem; // @[npc.scala 746:28]
  assign io_NPC_LSU_O_memRW = npcb_LSU_io_LSU_O_memRW; // @[npc.scala 747:24]
  assign io_NPC_LSU_O_memW = npcb_LSU_io_LSU_O_memW; // @[npc.scala 749:23]
  assign io_NPC_LSU_O_len = npcb_LSU_io_LSU_O_len; // @[npc.scala 745:22]
  assign io_NPC_GPRchanged = npcb_IDU_io_IDU_O_GPRneedWriteBack; // @[npc.scala 762:23]
  assign io_NPC_halt = npcb_IDU_io_IDU_O_halt; // @[npc.scala 729:17]
  assign io_NPC_error = npcb_IDU_io_IDU_O_error | npcb_EXU_io_EXU_O_error | npcb_LSU_io_LSU_O_error; // @[npc.scala 767:99]
  assign io_NPC_GPR00 = 64'h0; // @[npc.scala 714:37]
  assign io_NPC_GPR01 = GPR_io_NPC_GPR01_MPORT_data; // @[npc.scala 714:37]
  assign io_NPC_GPR02 = GPR_io_NPC_GPR02_MPORT_data; // @[npc.scala 714:37]
  assign io_NPC_GPR03 = GPR_io_NPC_GPR03_MPORT_data; // @[npc.scala 714:37]
  assign io_NPC_GPR04 = GPR_io_NPC_GPR04_MPORT_data; // @[npc.scala 714:37]
  assign io_NPC_GPR05 = GPR_io_NPC_GPR05_MPORT_data; // @[npc.scala 714:37]
  assign io_NPC_GPR06 = GPR_io_NPC_GPR06_MPORT_data; // @[npc.scala 714:37]
  assign io_NPC_GPR07 = GPR_io_NPC_GPR07_MPORT_data; // @[npc.scala 714:37]
  assign io_NPC_GPR08 = GPR_io_NPC_GPR08_MPORT_data; // @[npc.scala 714:37]
  assign io_NPC_GPR09 = GPR_io_NPC_GPR09_MPORT_data; // @[npc.scala 714:37]
  assign io_NPC_GPR10 = GPR_io_NPC_GPR10_MPORT_data; // @[npc.scala 714:37]
  assign io_NPC_GPR11 = GPR_io_NPC_GPR11_MPORT_data; // @[npc.scala 714:37]
  assign io_NPC_GPR12 = GPR_io_NPC_GPR12_MPORT_data; // @[npc.scala 714:37]
  assign io_NPC_GPR13 = GPR_io_NPC_GPR13_MPORT_data; // @[npc.scala 714:37]
  assign io_NPC_GPR14 = GPR_io_NPC_GPR14_MPORT_data; // @[npc.scala 714:37]
  assign io_NPC_GPR15 = GPR_io_NPC_GPR15_MPORT_data; // @[npc.scala 714:37]
  assign io_NPC_GPR16 = GPR_io_NPC_GPR16_MPORT_data; // @[npc.scala 714:37]
  assign io_NPC_GPR17 = GPR_io_NPC_GPR17_MPORT_data; // @[npc.scala 714:37]
  assign io_NPC_GPR18 = GPR_io_NPC_GPR18_MPORT_data; // @[npc.scala 714:37]
  assign io_NPC_GPR19 = GPR_io_NPC_GPR19_MPORT_data; // @[npc.scala 714:37]
  assign io_NPC_GPR20 = GPR_io_NPC_GPR20_MPORT_data; // @[npc.scala 714:37]
  assign io_NPC_GPR21 = GPR_io_NPC_GPR21_MPORT_data; // @[npc.scala 714:37]
  assign io_NPC_GPR22 = GPR_io_NPC_GPR22_MPORT_data; // @[npc.scala 714:37]
  assign io_NPC_GPR23 = GPR_io_NPC_GPR23_MPORT_data; // @[npc.scala 714:37]
  assign io_NPC_GPR24 = GPR_io_NPC_GPR24_MPORT_data; // @[npc.scala 714:37]
  assign io_NPC_GPR25 = GPR_io_NPC_GPR25_MPORT_data; // @[npc.scala 714:37]
  assign io_NPC_GPR26 = GPR_io_NPC_GPR26_MPORT_data; // @[npc.scala 714:37]
  assign io_NPC_GPR27 = GPR_io_NPC_GPR27_MPORT_data; // @[npc.scala 714:37]
  assign io_NPC_GPR28 = GPR_io_NPC_GPR28_MPORT_data; // @[npc.scala 714:37]
  assign io_NPC_GPR29 = GPR_io_NPC_GPR29_MPORT_data; // @[npc.scala 714:37]
  assign io_NPC_GPR30 = GPR_io_NPC_GPR30_MPORT_data; // @[npc.scala 714:37]
  assign io_NPC_GPR31 = GPR_io_NPC_GPR31_MPORT_data; // @[npc.scala 714:37]
  assign npcb_IFU_io_IFU_I_inst = io_NPC_getInst; // @[npc.scala 718:28]
  assign npcb_IDU_io_IDU_I_inst = npcb_IFU_io_IFU_I_inst; // @[npc.scala 724:28]
  assign npcb_EXU_io_EXU_I_src1 = npcb_IDU_io_IDU_O_rs1 == 5'h0 ? 64'h0 : GPR_npcb_src1_MPORT_data; // @[npc.scala 714:37]
  assign npcb_EXU_io_EXU_I_src2 = npcb_IDU_io_IDU_O_rs2 == 5'h0 ? 64'h0 : GPR_npcb_src2_MPORT_data; // @[npc.scala 714:37]
  assign npcb_EXU_io_EXU_I_imm = npcb_IDU_io_IDU_O_imm; // @[npc.scala 735:27]
  assign npcb_EXU_io_EXU_I_opcode = npcb_IDU_io_IDU_O_EXUopcode; // @[npc.scala 736:30]
  assign npcb_EXU_io_EXU_I_currentPC = PC; // @[npc.scala 737:33]
  assign npcb_LSU_io_LSU_I_src1 = npcb_EXU_io_EXU_O_result; // @[npc.scala 741:28]
  assign npcb_LSU_io_LSU_I_src2 = npcb_IDU_io_IDU_O_rs2 == 5'h0 ? 64'h0 : GPR_npcb_src2_MPORT_data; // @[npc.scala 714:37]
  assign npcb_LSU_io_LSU_I_opcode = npcb_IDU_io_IDU_O_LSUopcode; // @[npc.scala 744:30]
  assign npcb_LSU_io_LSU_I_memR = io_NPC_LSU_I_memR; // @[npc.scala 750:28]
  assign npcb_WBU_io_WBU_I_EXUresult = npcb_EXU_io_EXU_O_result; // @[npc.scala 756:33]
  assign npcb_WBU_io_WBU_I_LSUresult = npcb_LSU_io_LSU_O_result; // @[npc.scala 757:33]
  assign npcb_WBU_io_WBU_I_LSUenable = ~(npcb_IDU_io_IDU_O_LSUopcode == 6'h0); // @[npc.scala 755:36]
  assign npcb_WBU_io_WBU_I_rd = npcb_IDU_io_IDU_O_rd; // @[npc.scala 761:26]
  assign npcb_PCU_io_PCU_I_CurrentPC = PC; // @[npc.scala 758:33]
  assign npcb_PCU_io_PCU_I_imm = npcb_IDU_io_IDU_O_imm; // @[npc.scala 760:27]
  assign npcb_PCU_io_PCU_I_willJump = ~npcb_IDU_io_IDU_O_snpcISdnpc & npcb_EXU_io_EXU_O_snpcNEQdnpc; // @[npc.scala 759:68]
  always @(posedge clock) begin
    if (GPR_MPORT_en & GPR_MPORT_mask) begin
      GPR[GPR_MPORT_addr] <= GPR_MPORT_data; // @[npc.scala 713:18]
    end
    if (reset) begin // @[npc.scala 711:21]
      PC <= io_NPC_startPC; // @[npc.scala 711:21]
    end else begin
      PC <= npcb_PCU_io_PCU_O_DynamicNextPC; // @[npc.scala 764:8]
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
