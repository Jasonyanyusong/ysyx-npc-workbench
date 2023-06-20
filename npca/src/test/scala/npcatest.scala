package npca
import chisel3.stage.ChiselGeneratorAnnotation

object npcaMain extends App{
    (new chisel3.stage.ChiselStage).execute(Array("--target-dir", "generated/npca"), Seq(ChiselGeneratorAnnotation(() => new npca)))
}