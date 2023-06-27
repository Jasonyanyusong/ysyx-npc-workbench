package npca
import chisel3.stage.ChiselGeneratorAnnotation

object npcMain extends App{
    (new chisel3.stage.ChiselStage).execute(Array("--target-dir", "generated/npc"), Seq(ChiselGeneratorAnnotation(() => new npc)))
}