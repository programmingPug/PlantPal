// update-nickname-dialog.component.ts
import { Component, Inject } from '@angular/core';
import { MatDialogRef, MAT_DIALOG_DATA } from '@angular/material/dialog';
import { FormBuilder, FormGroup, Validators, ReactiveFormsModule } from '@angular/forms';
import { CommonModule } from '@angular/common';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatInputModule } from '@angular/material/input';
import { MatButtonModule } from '@angular/material/button';
import { MatDialogModule } from '@angular/material/dialog';

export interface UpdateNicknameData {
  name: string;
  nickname?: string;
}

@Component({
  selector: 'app-update-nickname-dialog',
  templateUrl: './update-nickname-dialog.component.html',
  styleUrls: ['./update-nickname-dialog.component.scss'],
  standalone: true,
  imports: [
    CommonModule,
    ReactiveFormsModule,
    MatFormFieldModule,
    MatInputModule,
    MatButtonModule,
    MatDialogModule
  ]
})
export class UpdateNicknameDialogComponent {
  nicknameForm: FormGroup;

  constructor(
    public dialogRef: MatDialogRef<UpdateNicknameDialogComponent>,
    @Inject(MAT_DIALOG_DATA) public data: UpdateNicknameData,
    private fb: FormBuilder
  ) {
    this.nicknameForm = this.fb.group({
      nickname: [
        data.nickname || '',
        [
          Validators.required,
          Validators.minLength(3),
          Validators.maxLength(20),
          Validators.pattern('^[a-zA-Z0-9 _-]+$') // Allows letters, numbers, spaces, underscores, hyphens
        ]
      ]
    });
  }

  onSave(): void {
    if (this.nicknameForm.valid) {
      this.dialogRef.close({ nickname: this.nicknameForm.value.nickname });
    }
  }

  onCancel(): void {
    this.dialogRef.close();
  }
}