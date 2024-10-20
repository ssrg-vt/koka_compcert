(* *********************************************************************)
(*                                                                     *)
(*              The Compcert verified compiler                         *)
(*                                                                     *)
(*                 Xavier Leroy, INRIA Paris                           *)
(*           Prashanth Mundkur, SRI International                      *)
(*                                                                     *)
(*  Copyright Institut National de Recherche en Informatique et en     *)
(*  Automatique.  All rights reserved.  This file is distributed       *)
(*  under the terms of the INRIA Non-Commercial License Agreement.     *)
(*                                                                     *)
(*  The contributions by Prashanth Mundkur are reused and adapted      *)
(*  under the terms of a Contributor License Agreement between         *)
(*  SRI International and INRIA.                                       *)
(*                                                                     *)
(* *********************************************************************)

(** Correctness of instruction selection for 64-bit integer operations *)

Require Import String Coqlib Maps Integers Floats Errors.
Require Archi.
Require Import AST Values Memory Globalenvs Events.
Require Import Cminor Op CminorSel.
Require Import SelectOp SelectOpproof SplitLong SplitLongproof.
Require Import SelectLong.

Local Open Scope cminorsel_scope.
Local Open Scope string_scope.

(** * Correctness of the instruction selection functions for 64-bit operators *)

Section CMCONSTR.

Variable prog: program.
Variable hf: helper_functions.
Hypothesis HELPERS: helper_functions_declared prog hf.
Let ge := Genv.globalenv prog.
Variable sp: val.
Variable e: env.
Variable m: mem.

Definition unary_constructor_sound (cstr: expr -> expr) (sem: val -> val) : Prop :=
  forall le a x,
  eval_expr ge sp e m le a x ->
  exists v, eval_expr ge sp e m le (cstr a) v /\ Val.lessdef (sem x) v.

Definition binary_constructor_sound (cstr: expr -> expr -> expr) (sem: val -> val -> val) : Prop :=
  forall le a x b y,
  eval_expr ge sp e m le a x ->
  eval_expr ge sp e m le b y ->
  exists v, eval_expr ge sp e m le (cstr a b) v /\ Val.lessdef (sem x y) v.

Definition partial_unary_constructor_sound (cstr: expr -> expr) (sem: val -> option val) : Prop :=
  forall le a x y,
  eval_expr ge sp e m le a x ->
  sem x = Some y ->
  exists v, eval_expr ge sp e m le (cstr a) v /\ Val.lessdef y v.

Definition partial_binary_constructor_sound (cstr: expr -> expr -> expr) (sem: val -> val -> option val) : Prop :=
  forall le a x b y z,
  eval_expr ge sp e m le a x ->
  eval_expr ge sp e m le b y ->
  sem x y = Some z ->
  exists v, eval_expr ge sp e m le (cstr a b) v /\ Val.lessdef z v.


Theorem eval_addl: binary_constructor_sound addl Val.addl.
Proof.
  unfold addl. destruct Archi.splitlong eqn:SL.
  - unfold binary_constructor_sound. intros.
    apply eval_splitlong2; auto.
    + intros.
      eexists; split.
      { unfold addl_split.
        repeat econstructor ; eauto; apply eval_lift ; eauto.
      }
      intros; subst.
      unfold Val.addl.
      rewrite Int64.decompose_add_no_carry.
      simpl.
      destruct (Int.ltu (Int.add p2 q2) p2).
      unfold Vtrue. reflexivity.
      unfold Vfalse. reflexivity.
    + destruct x, y; auto.
  -  apply SplitLongproof.eval_addl. apply Archi.splitlong_ptr32; auto.
Qed.

Theorem eval_subl: binary_constructor_sound subl Val.subl.
Proof.
  unfold subl. destruct Archi.splitlong eqn:SL.
  - unfold binary_constructor_sound. intros.
    apply eval_splitlong2; auto.
    + intros.
      eexists; split.
      { unfold subl_split.
        repeat econstructor ; eauto.
      }
      intros; subst.
      simpl.
      rewrite Int64.decompose_sub_no_carry.
      simpl.
      destruct (Int.ltu p2 q2).
      unfold Vtrue. reflexivity.
      unfold Vfalse. reflexivity.
    + destruct x, y; auto.
  -  apply SplitLongproof.eval_subl. apply Archi.splitlong_ptr32; auto.
Qed.



End CMCONSTR.
