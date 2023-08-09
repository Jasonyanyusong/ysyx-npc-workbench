package npc
import chisel3.stage.ChiselGeneratorAnnotation

object npcMain extends App{
    (new chisel3.stage.ChiselStage).execute(Array("--target-dir", "generated/npc"), Seq(ChiselGeneratorAnnotation(() => new NPC)))

    //(new chisel3.stage.ChiselStage).execute(Array("--target-dir", "generated/parts"), Seq(ChiselGeneratorAnnotation(() => new IFU)))
    //(new chisel3.stage.ChiselStage).execute(Array("--target-dir", "generated/parts"), Seq(ChiselGeneratorAnnotation(() => new IDU)))
    //(new chisel3.stage.ChiselStage).execute(Array("--target-dir", "generated/parts"), Seq(ChiselGeneratorAnnotation(() => new EXU)))
    //(new chisel3.stage.ChiselStage).execute(Array("--target-dir", "generated/parts"), Seq(ChiselGeneratorAnnotation(() => new LSU)))
    //(new chisel3.stage.ChiselStage).execute(Array("--target-dir", "generated/parts"), Seq(ChiselGeneratorAnnotation(() => new WBU)))
}