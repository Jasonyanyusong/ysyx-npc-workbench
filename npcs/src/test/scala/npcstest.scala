package npcs
import chisel3.stage.ChiselGeneratorAnnotation

object npcsMain extends App{
    (new chisel3.stage.ChiselStage).execute(Array("--target-dir", "generated/npcs"), Seq(ChiselGeneratorAnnotation(() => new npcs)))
}