package npcb
import chisel3.stage.ChiselGeneratorAnnotation

object npcbMain extends App{
    (new chisel3.stage.ChiselStage).execute(Array("--target-dir", "generated/npcb"), Seq(ChiselGeneratorAnnotation(() => new npcb)))
}