package npc
import chisel3.stage.ChiselGeneratorAnnotation

import npc.units.iFetch._
import npc.units.iDecode._
import npc.units.iExecute._
import npc.units.iLoadStore._
import npc.units.iWriteBack._

object npcMain extends App{
    (new chisel3.stage.ChiselStage).execute(Array("--target-dir", "generated/npc"), Seq(ChiselGeneratorAnnotation(() => new NPC)))
}