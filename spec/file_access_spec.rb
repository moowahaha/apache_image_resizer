require File.join(File.dirname(__FILE__), 'spec_helper')

describe "file access" do
  it "should 404 when accessing a file that doesn't exist" do
    lambda {
      @resizer.resize(
              :padding_color => 'ffffff',
              :filename => 'doesnotexist.jpg',
              :width => 20,
              :height => 20
      )
    }.should raise_error(/Status: 404 No such file or directory: doesnotexist.jpg/)
  end

  it "should 403 when it doesn't have permission to access a file" do
    File.chmod(0222, File.join(File.dirname(__FILE__), 'fixtures', 'noreadpermission.jpg'))

    lambda {
      @resizer.resize(
              :padding_color => 'ffffff',
              :filename => 'noreadpermission.jpg',
              :width => 20,
              :height => 20
      )
    }.should raise_error(/Status: 403 Permission denied: noreadpermission.jpg/)

    File.chmod(0666, File.join(File.dirname(__FILE__), 'fixtures', 'noreadpermission.jpg'))
  end

  it "should 403 when trying to use double-dots" do
    lambda {
      @resizer.resize(
              :padding_color => 'ffffff',
              :filename => '/../blah.jpg',
              :width => 20,
              :height => 20
      )
    }.should raise_error(/Status: 403 Not allowed/)
  end
end