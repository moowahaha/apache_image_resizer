require File.join(File.dirname(__FILE__), 'spec_helper')

describe "running environment" do
  it "should throw an error if the environment is not setup right" do
    lambda {
      @resizer.exec(@resizer.resizer_path)
    }.should raise_error(
            /Missing environment settings: IMAGE_ROOT PADDING_COLOR MAX_HEIGHT MAX_WIDTH\nStatus: 500 Not setup right\!/
    )
  end
end