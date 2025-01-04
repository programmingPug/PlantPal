import { ComponentFixture, TestBed } from '@angular/core/testing';

import { UpdateNicknameDialogComponent } from './update-nickname-dialog.component';

describe('UpdateNicknameDialogComponent', () => {
  let component: UpdateNicknameDialogComponent;
  let fixture: ComponentFixture<UpdateNicknameDialogComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [UpdateNicknameDialogComponent]
    })
    .compileComponents();

    fixture = TestBed.createComponent(UpdateNicknameDialogComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
